#include "__internalConfig/LoggerConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ScriptVM.h"
#include "Base/IO/Files/FileUtils.h"
#include "Service/ScriptNameBuilder.h"
#include "NodeValue/ScriptAST.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "DevModule.h"
#include <cstdlib>
#include <regex>
#include <filesystem>
#include <unistd.h>

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
	m_devModule = std::make_unique<DevModule>(*this);
	addConsolePrelude("Dev = import \"bind:ska.dev\"");
	DevConsole::get().setEvaluator(this);
}

ska::scripting::ScriptVM::Modules& ska::scripting::ScriptVM::WithStandardLibraryPath(Modules& modules, std::string standardLibraryPath) {
	// Set before the creation of the first binding module (its script template is a standard library script)
	ScriptNameStandardLibraryPath() = std::move(standardLibraryPath);
	return modules;
}

ska::scripting::ScriptVM::~ScriptVM() {
	DevConsole::get().removeEvaluator(this);
	// The coroutines are released before the executor they run on
	m_suspended.clear();
}

const ska::scripting::ScriptContext& ska::scripting::ScriptVM::context() const {
	return m_runningContext;
}

std::optional<std::size_t> ska::scripting::ScriptVM::compile(const std::string& scriptFile, bool quiet) {
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
		m_lastCompileError = "script file not found : \"" + scriptFile + "\"";
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
		m_lastCompileError = e.what();
		if (!quiet) {
			LOG_ERROR << "Script \"" << scriptFile << "\" could not be compiled : " << e.what();
		}
		m_failedScripts.insert(name);
		return {};
	}
}

bool ska::scripting::ScriptVM::start(const std::string& scriptFile, std::vector<std::string> arguments, ScriptContext context) {
	if (context.name.empty()) {
		context.name = scriptFile;
	}
	return start(scriptFile, std::move(arguments), std::move(context), EndCallback{});
}

bool ska::scripting::ScriptVM::start(const std::string& scriptFile, std::vector<std::string> arguments, ScriptContext context, EndCallback ended) {
	if (context.name.empty()) {
		context.name = scriptFile;
	}
	auto script = PendingStart{ scriptFile, std::move(arguments), std::move(context), NodeValue{}, {}, std::move(ended) };
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
				if (script.ended) {
					script.ended(false, "\"" + script.scriptFile + "\" could not be compiled : " + m_lastCompileError);
				}
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
		auto running = std::make_unique<RunningScript>(RunningScript{ std::move(coroutine), std::move(script.context), Key(script.scriptFile, script.arguments), {}, {}, std::move(script.ended) });
		if (afterStep(*running)) {
			m_suspended.push_back(std::move(running));
		} else if (running->ended) {
			running->ended(true, endText(*running));
		}
		return true;
	} catch (std::exception& e) {
		m_wait.reset();
		LOG_ERROR << "Error in script \"" << script.context.name << "\" : " << e.what();
		if (script.ended) {
			script.ended(false, std::string{ "error in script \"" } + script.context.name + "\" : " + e.what());
		}
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
			script->failed = true;
			if (script->ended) {
				script->ended(false, std::string{ "error in script \"" } + script->context.name + "\" : " + e.what());
				script->ended = {};
			}
		}
		afterStep(*script);
		if (!script->coroutine.suspended() && script->ended) {
			script->ended(true, endText(*script));
			script->ended = {};
		}
	}

	m_suspended.erase(std::remove_if(m_suspended.begin(), m_suspended.end(), [](const auto& script) {
		return script->failed || !script->coroutine.suspended();
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
			started.push_back(PendingStart{ "", {}, context, hook.function, arguments, {} });
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

/* Development console */

namespace {
	std::string ValueText(const ska::NodeValue& value) {
		if (value.is<ska::StringShared>()) { return *value.nodeval<ska::StringShared>(); }
		if (value.is<long>()) { return std::to_string(value.nodeval<long>()); }
		if (value.is<double>()) { auto text = std::stringstream{}; text << value.nodeval<double>(); return text.str(); }
		if (value.is<bool>()) { return value.nodeval<bool>() ? "true" : "false"; }
		return "";
	}
}

std::string ska::scripting::ScriptVM::endText(RunningScript& script) {
	auto text = std::string{};
	try {
		text = ValueText(script.coroutine.result());
	} catch (std::exception&) {
		// No value returned
	}
	if (text.empty() && script.context.reply != nullptr) {
		text = *script.context.reply;
	}
	return text;
}

void ska::scripting::ScriptVM::addConsolePrelude(std::string line) {
	if (std::find(m_consolePrelude.begin(), m_consolePrelude.end(), line) == m_consolePrelude.end()) {
		m_consolePrelude.push_back(std::move(line));
	}
}

std::string ska::scripting::ScriptVM::writeConsoleScript(const std::string& body) {
	// Next to the console socket (the run folder of tools/ska-run), else in the temporary directory
	auto directory = std::filesystem::path{};
	const auto* socket = std::getenv("SKA_CONSOLE");
	if (socket != nullptr && *socket != '\0') {
		directory = std::filesystem::path{ socket }.parent_path() / "eval";
	} else {
		directory = std::filesystem::temp_directory_path() / ("ska-eval-" + std::to_string(::getpid()));
	}
	std::filesystem::create_directories(directory);
	const auto file = directory / ("eval_" + std::to_string(++m_consoleScripts) + ".miniska");
	auto out = std::ofstream{ file };
	for (const auto& line : m_consolePrelude) {
		out << line << "\n";
	}
	out << body;
	return file.string();
}

void ska::scripting::ScriptVM::eval(const std::string& code, DevReplier reply) {
	auto context = ScriptContext{};
	context.name = "console";
	context.reply = std::make_shared<std::string>();
	auto ended = [reply](bool ok, const std::string& text) {
		reply(ok ? DevReply::Value(text) : DevReply::Error(text));
	};

	// One line : maybe an expression, its value is the answer
	if (code.find('\n') == std::string::npos) {
		const auto expression = writeConsoleScript("run = function(args: string[]): string do\nreturn ((" + code + "): string)\nend\n");
		if (compile(expression, true).has_value()) {
			start(expression, {}, context, ended);
			return;
		}
	}
	// Statements
	const auto statements = writeConsoleScript("run = function(args: string[]) do\n" + code + "\nend\n");
	if (!compile(statements, true).has_value()) {
		// The lines of the error : those of the code sent (after the prelude and the "run" line)
		const auto offset = static_cast<long>(m_consolePrelude.size()) + 1;
		auto error = std::string{};
		const auto line = std::regex{ "\\(l\\.([0-9]+)" };
		auto last = std::sregex_iterator{};
		auto position = m_lastCompileError.cbegin();
		for (auto match = std::sregex_iterator{ m_lastCompileError.begin(), m_lastCompileError.end(), line }; match != last; ++match) {
			error.append(position, (*match)[0].first);
			error += "(l." + std::to_string(std::stol((*match)[1].str()) - offset);
			position = (*match)[0].second;
		}
		error.append(position, m_lastCompileError.cend());
		reply(DevReply::Error("does not compile : " + error));
		return;
	}
	start(statements, {}, context, ended);
}

void ska::scripting::ScriptVM::run(const std::string& file, std::vector<std::string> arguments, DevReplier reply) {
	auto context = ScriptContext{};
	context.name = file;
	context.reply = std::make_shared<std::string>();
	start(file, std::move(arguments), std::move(context), [reply](bool ok, const std::string& text) {
		reply(ok ? DevReply::Value(text) : DevReply::Error(text));
	});
}
