#include "__internalConfig/LoggerConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ScriptVM.h"
#include "Base/IO/Files/FileUtils.h"
#include "Service/ScriptNameBuilder.h"
#include "NodeValue/ScriptAST.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

namespace ska {
	namespace scripting {
		struct ScriptVMLog;
	}
}
SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::scripting::ScriptVMLog);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::scripting::ScriptVMLog)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::scripting::ScriptVMLog)

std::string ska::scripting::ScriptVM::DefaultStandardLibraryPath() {
	return FileNameData{ FileUtils::getExecutablePath() }.path + "/std";
}

ska::scripting::ScriptVM::ScriptVM(std::string standardLibraryPath) :
	m_parser(m_reservedKeywords),
	m_typeBuilder(m_parser, m_typeCrosser),
	m_typeChecker(m_parser, m_typeCrosser),
	m_generator(m_reservedKeywords),
	m_interpreter(m_parser, m_generator, m_reservedKeywords),
	m_modules(m_scriptCache.astCache, m_typeBuilder, m_typeChecker, m_reservedKeywords, m_parser, m_scriptCache, m_interpreter),
	m_logModule(WithStandardLibraryPath(m_modules, std::move(standardLibraryPath))),
	m_mathModule(m_modules) {
}

ska::scripting::ScriptVM::Modules& ska::scripting::ScriptVM::WithStandardLibraryPath(Modules& modules, std::string standardLibraryPath) {
	// Set before the creation of the first binding module (its script template is a standard library script)
	ScriptNameStandardLibraryPath() = std::move(standardLibraryPath);
	return modules;
}

ska::scripting::ScriptVM::~ScriptVM() {
	// The coroutines are released before the executor they run on
	m_suspended.clear();
}

const ska::scripting::ScriptContext& ska::scripting::ScriptVM::context() const {
	return m_runningContext;
}

std::optional<std::size_t> ska::scripting::ScriptVM::compile(const std::string& scriptFile) {
	const auto name = FileUtils::getCanonicalPath(scriptFile);
	auto compiled = m_compiledScripts.find(name);
	if (compiled != m_compiledScripts.end()) {
		return compiled->second;
	}

	if (m_failedScripts.find(name) != m_failedScripts.end()) {
		// Already reported (a script can be triggered at each frame)
		return {};
	}

	auto file = std::ifstream{ name };
	if (file.fail()) {
		LOG_ERROR << "Script file not found : \"" << scriptFile << "\"";
		return {};
	}
	try {
		auto ast = m_parser.subParse(m_scriptCache.astCache, name, file);
		const auto& generated = m_generator.generate(m_scriptCache, bytecode::ScriptGenerationHelper{ m_scriptCache, *ast });
		const auto index = generated.id();
		m_compiledScripts.emplace(name, index);
		return index;
	} catch (std::exception& e) {
		LOG_ERROR << "Script \"" << scriptFile << "\" could not be compiled : " << e.what();
		m_failedScripts.insert(name);
		return {};
	}
}

bool ska::scripting::ScriptVM::start(const std::string& scriptFile, std::vector<std::string> arguments, ScriptContext context) {
	if (context.name.empty()) {
		context.name = scriptFile;
	}
	auto script = PendingStart{ scriptFile, std::move(arguments), std::move(context) };
	if (m_scriptRunning) {
		// A coroutine cannot start another one : it starts after the current step of the running script
		m_pendingStarts.push_back(std::move(script));
		return true;
	}
	const auto started = startNow(std::move(script));
	startPendingScripts();
	return started;
}

bool ska::scripting::ScriptVM::startNow(PendingStart script) {
	try {
		auto function = std::move(script.function);
		auto values = std::move(script.values);
		if (function.empty()) {
			// A script file : its "run" function, with the string arguments
			const auto scriptIndex = compile(script.scriptFile);
			if (!scriptIndex.has_value()) {
				return false;
			}
			function = m_interpreter.exported(m_executor, *scriptIndex, m_scriptCache, "run");

			auto arguments = std::make_shared<NodeValueArrayRaw>();
			for (const auto& argument : script.arguments) {
				arguments->push_back(NodeValue{ std::make_shared<std::string>(argument) });
			}
			values = { NodeValue{ std::move(arguments) } };
		}

		LOG_INFO << "Starting script \"" << script.context.name << "\"";
		auto coroutine = runStep(script.context, [&]() {
			return m_interpreter.start(m_executor, m_scriptCache, function, std::move(values));
		});
		auto running = std::make_unique<RunningScript>(RunningScript{ std::move(coroutine), std::move(script.context), Key(script.scriptFile, script.arguments), {}, {} });
		if (afterStep(*running)) {
			m_suspended.push_back(std::move(running));
		}
		return true;
	} catch (std::exception& e) {
		m_wait.reset();
		LOG_ERROR << "Error in script \"" << script.context.name << "\" : " << e.what();
		return false;
	}
}

bool ska::scripting::ScriptVM::afterStep(RunningScript& script) {
	auto wait = std::move(m_wait);
	m_wait.reset();
	if (!script.coroutine.suspended()) {
		return false;
	}
	if (wait.has_value()) {
		script.ready = std::move(wait->ready);
		script.value = std::move(wait->value);
	} else {
		// Suspended without condition : resumed at the next update
		script.ready = {};
		script.value = {};
	}
	return true;
}

void ska::scripting::ScriptVM::startPendingScripts() {
	while (!m_pendingStarts.empty()) {
		auto pending = std::move(m_pendingStarts);
		m_pendingStarts.clear();
		for (auto& script : pending) {
			startNow(std::move(script));
		}
	}
}

void ska::scripting::ScriptVM::update(unsigned int elapsedTime) {
	m_time += elapsedTime;

	// Scripts suspended during this update are resumed at the next one
	auto toResume = std::vector<RunningScript*>{};
	for (auto& script : m_suspended) {
		toResume.push_back(script.get());
	}

	for (auto* script : toResume) {
		if (script->ready && !script->ready()) {
			continue;
		}
		auto value = script->value ? script->value() : NodeValue{};
		try {
			runStep(script->context, [&]() {
				m_interpreter.resume(script->coroutine, std::move(value));
				return 0;
			});
		} catch (std::exception& e) {
			m_wait.reset();
			LOG_ERROR << "Error in script \"" << script->context.name << "\" : " << e.what();
		}
		afterStep(*script);
	}

	m_suspended.erase(std::remove_if(m_suspended.begin(), m_suspended.end(), [](const auto& script) {
		return !script->coroutine.suspended();
	}), m_suspended.end());

	startPendingScripts();
}

void ska::scripting::ScriptVM::suspendUntil(std::function<bool()> ready, std::function<NodeValue()> value) {
	m_interpreter.suspend();
	m_wait = Wait{ std::move(ready), std::move(value) };
}

void ska::scripting::ScriptVM::wait(unsigned int milliseconds) {
	const auto end = m_time + milliseconds;
	suspendUntil([this, end]() { return m_time >= end; });
}

std::string ska::scripting::ScriptVM::Key(const std::string& scriptFile, const std::vector<std::string>& arguments) {
	auto key = FileUtils::getCanonicalPath(scriptFile);
	for (const auto& argument : arguments) {
		key += '\0' + argument;
	}
	return key;
}

bool ska::scripting::ScriptVM::running(const std::string& scriptFile, const std::vector<std::string>& arguments) const {
	const auto key = Key(scriptFile, arguments);
	return std::any_of(m_suspended.begin(), m_suspended.end(), [&key](const auto& script) {
		return script->key == key && script->coroutine.suspended();
	});
}

void ska::scripting::ScriptVM::start(const ScriptSleepComponent& script, EntityId origin) {
	// One running instance of a script file started by the triggers (as the legacy scripts) : the arguments of a trigger
	// can change while the script runs (ex : the position of the player walking on a block)
	const auto name = FileUtils::getCanonicalPath(script.name);
	const auto alreadyRunning = std::any_of(m_suspended.begin(), m_suspended.end(), [&name](const auto& running) {
		return running->coroutine.suspended() && running->key.compare(0, name.size(), name) == 0 &&
			(running->key.size() == name.size() || running->key[name.size()] == '\0');
	});
	if (alreadyRunning) {
		return;
	}
	auto context = ScriptContext{};
	context.name = script.name;
	context.origin = origin;
	start(script.name, script.args, std::move(context));
}

/* Synchronous calls */

namespace {
	ska::ExpressionType ValueType(const ska::NodeValue& value) {
		if (value.is<long>()) { return ska::ExpressionType::INT; }
		if (value.is<double>()) { return ska::ExpressionType::FLOAT; }
		if (value.is<bool>()) { return ska::ExpressionType::BOOLEAN; }
		if (value.is<ska::StringShared>()) { return ska::ExpressionType::STRING; }
		if (value.is<ska::NodeValueArray>()) { return ska::ExpressionType::ARRAY; }
		if (value.is<ska::ScriptVariableRef>()) { return ska::ExpressionType::FUNCTION; }
		return ska::ExpressionType::OBJECT;
	}
}

const ska::Symbol* ska::scripting::ScriptVM::exportedSymbol(std::size_t scriptIndex, const std::string& name) {
	for (const auto& exported : m_scriptCache.getExportedSymbols(scriptIndex)) {
		if (exported.has_value() && exported->symbol != nullptr && exported->symbol->name() == name) {
			return exported->symbol;
		}
	}
	return nullptr;
}

ska::NodeValue ska::scripting::ScriptVM::callFunction(const std::string& scriptFile, const std::string& function, std::vector<NodeValue> arguments, ExpressionType returned) {
	const auto name = scriptFile + " : " + function;
	if (m_scriptRunning) {
		throw std::runtime_error("cannot call \"" + name + "\" while a script is running (the engine calls the scripts between their steps)");
	}
	const auto scriptIndex = compile(scriptFile);
	if (!scriptIndex.has_value()) {
		throw std::runtime_error("cannot call \"" + name + "\" : the script could not be compiled");
	}

	// The function type : its parameters, then its returned type
	const auto* symbol = exportedSymbol(*scriptIndex, function);
	if (symbol == nullptr || symbol->type() != ExpressionType::FUNCTION) {
		throw std::runtime_error("cannot call \"" + name + "\" : the script has no function named \"" + function + "\"");
	}
	const auto& type = symbol->type();
	auto signature = std::stringstream{};
	signature << type;
	if (type.size() != arguments.size() + 1) {
		throw std::runtime_error("cannot call \"" + name + "\" (" + signature.str() + ") with " + std::to_string(arguments.size()) + " argument(s)");
	}
	for (auto index = std::size_t{ 0 }; index < arguments.size(); index++) {
		if (ValueType(arguments[index]) != type[index].type()) {
			throw std::runtime_error("cannot call \"" + name + "\" (" + signature.str() + ") : the argument " + std::to_string(index + 1) +
				" is not a " + ExpressionTypeSTR[static_cast<std::size_t>(type[index].type())]);
		}
	}
	if (type.back().type() != returned) {
		throw std::runtime_error("cannot call \"" + name + "\" (" + signature.str() + ") : the engine expects a " +
			ExpressionTypeSTR[static_cast<std::size_t>(returned)] + " returned value");
	}

	auto context = ScriptContext{};
	context.name = name;
	auto value = runStep(context, [&]() {
		auto functionValue = m_interpreter.exported(m_executor, *scriptIndex, m_scriptCache, function);
		auto coroutine = m_interpreter.start(m_executor, m_scriptCache, functionValue, std::move(arguments));
		const auto waited = m_wait.has_value() || coroutine.suspended();
		m_wait.reset();
		if (waited) {
			throw std::runtime_error("the function \"" + name + "\" cannot wait : it is called by the engine, that needs its value right away");
		}
		return coroutine.result();
	});
	// Scripts started by the function (Engine.script)
	startPendingScripts();
	return value;
}

/* Hooks */

void ska::scripting::ScriptVM::declareEvent(std::string event) {
	m_events.insert(std::move(event));
}

bool ska::scripting::ScriptVM::eventDeclared(const std::string& event) const {
	return m_events.find(event) != m_events.end();
}

std::size_t ska::scripting::ScriptVM::addHook(const std::string& event, NodeValue function) {
	if (!eventDeclared(event)) {
		auto events = std::vector<std::string>{ m_events.begin(), m_events.end() };
		std::sort(events.begin(), events.end());
		auto list = std::string{};
		for (const auto& known : events) {
			list += (list.empty() ? "" : ", ") + known;
		}
		throw std::runtime_error("unknown event \"" + event + "\" (events : " + (list.empty() ? "none" : list) + ")");
	}
	if (!function.is<ScriptVariableRef>()) {
		throw std::runtime_error("a hook is a script function");
	}
	const auto id = m_nextHook++;
	m_hooks.push_back(Hook{ id, event, std::move(function) });
	return id;
}

bool ska::scripting::ScriptVM::removeHook(std::size_t hook) {
	const auto size = m_hooks.size();
	m_hooks.erase(std::remove_if(m_hooks.begin(), m_hooks.end(), [hook](const Hook& h) { return h.id == hook; }), m_hooks.end());
	return m_hooks.size() != size;
}

void ska::scripting::ScriptVM::clearHooks() {
	m_hooks.clear();
}

std::size_t ska::scripting::ScriptVM::hooks(const std::string& event) const {
	return static_cast<std::size_t>(std::count_if(m_hooks.begin(), m_hooks.end(), [&event](const Hook& h) { return h.event == event; }));
}

std::size_t ska::scripting::ScriptVM::emit(const std::string& event, std::vector<NodeValue> arguments, ScriptContext context) {
	if (!eventDeclared(event)) {
		throw std::runtime_error("unknown event \"" + event + "\" emitted (not declared)");
	}
	if (context.name.empty()) {
		context.name = "hook \"" + event + "\"";
	}
	// The hooks list can change while the hooks run (a hook removing itself)
	auto started = std::vector<PendingStart>{};
	for (const auto& hook : m_hooks) {
		if (hook.event == event) {
			started.push_back(PendingStart{ "", {}, context, hook.function, arguments });
		}
	}
	for (auto& hook : started) {
		m_pendingStarts.push_back(std::move(hook));
	}
	if (!m_scriptRunning) {
		startPendingScripts();
	}
	return started.size();
}
