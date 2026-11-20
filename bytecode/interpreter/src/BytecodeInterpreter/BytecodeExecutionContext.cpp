#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"

#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ExecutionContext);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ExecutionContext)

ska::bytecode::ExecutionContext::ExecutionContext(ExecutionContext& old, std::size_t scriptIndex) :
				ExecutionContext(old.m_out, scriptIndex, old.m_in) { }

ska::bytecode::ExecutionContext::ExecutionContext(Executor& container, std::size_t scriptIndex, GenerationOutput& instructions) :
	m_out(container),
	m_in(instructions),
	m_current(&container.scriptOrNew(instructions, scriptIndex)) {
}

ska::bytecode::ScriptExecutionOutput ska::bytecode::ExecutionContext::generateExportedVariables(std::size_t scriptIndex) {
	auto& scriptExecution = m_out.scriptOrThrow(scriptIndex);
	const auto& symbols = scriptExecution.exports();
	if(symbols == nullptr) {
		auto result = std::make_shared<NodeValueArrayRaw>();
		const auto& exportedSymbolsVariables = m_in.getExportedSymbols(scriptIndex);
		for (const auto& variable : exportedSymbolsVariables) {
			result->push_back(getCell(variable.value().value));
		}
		scriptExecution.setExportsSection(result);
		return result;
	}
	SLOG(ska::LogLevel::Info) << "No generation of exported symbols for script \"" << scriptIndex << "\" required";
	return symbols;
}

bool ska::bytecode::ExecutionContext::isGenerated(std::size_t scriptIndex) const {
	return m_in.isGenerated(scriptIndex);
}

ska::bytecode::ExecutionContext ska::bytecode::ExecutionContext::buildContextForScript(const ScriptVariableRef& value) {
	return ExecutionContext{ *this, value.script };
}

void ska::bytecode::ExecutionContext::jumpAbsolute(NodeValue& value) {
	const auto& variable = value.nodeval<ScriptVariableRef>();
	auto context = buildContextForScript(variable);
	m_out.callStack(m_current->snapshot(value));
	m_current = context.m_current;
	m_current->jumpAbsolute(variable.variable - 1);
}

ska::NodeValue ska::bytecode::ExecutionContext::getCell(const Operand &variable) const {
	return m_out.resolveValue(variable, *m_current);
}

void ska::bytecode::ExecutionContext::release(const Operand& dest) {
	m_out.release(dest, *m_current);
}

ska::NodeValue ska::bytecode::ExecutionContext::jumpReturn(NodeValue& returnedValue) {
	auto returnedCallStackValue = m_out.returnCallStack();
	auto whereToGo = returnedCallStackValue.nodeval<ScriptVariableRef>();


	LOG_DEBUG << "Returning to instruction index " << whereToGo.variable << " in script " << whereToGo.script;
	auto context = buildContextForScript(whereToGo);
	m_current = context.m_current;
	m_current->jumpAbsolute(whereToGo.variable);

	// A returned value that already owns an env (ex : an existing instance returned by a method) must keep it.
	// Only values without env (ex : an object freshly created by a factory) inherit the env of the returning call.
	if (returnedValue.hasEnv()) {
		return returnedValue;
	}
	return NodeValue {returnedCallStackValue, returnedValue};
}

const ska::NativeFunction& ska::bytecode::ExecutionContext::getBinding(const ScriptVariableRef& bindingRef) const {
	return m_in.getBinding(bindingRef);
}

void ska::bytecode::ExecutionContext::generate(StatementParser& parser, Generator& generator) {
	if (!m_in.exist(currentScriptId())) {
		auto* name = m_in.findKey(currentScriptId());
		if (name == nullptr) {
			throw std::runtime_error("bad script id (key not found) : " + std::to_string(currentScriptId()));
		}
		auto file = std::ifstream { *name };
		auto scriptAst = parser.subParse(m_in.astCache, *name, file);
		generator.generate(m_in, ScriptGenerationHelper{ m_in, *scriptAst });
	} else {
		generator.generate(m_in, currentScriptId());
	}
}
