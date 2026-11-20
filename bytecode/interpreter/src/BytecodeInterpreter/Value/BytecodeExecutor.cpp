#include "Config/LoggerConfigLang.h"
#include "BytecodeExecutor.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Executor);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Executor)


void ska::bytecode::Executor::callStack(NodeValue value) {
	callstack.push_back(std::move(value));
}

ska::NodeValue ska::bytecode::Executor::returnCallStack() {
	assert(!callstack.empty());

	auto result = std::move(callstack.back());
	LOG_DEBUG << "Poping env: " << result.printEnv();
	callstack.pop_back();
	return result;
}

const ska::NodeValue *ska::bytecode::Executor::useFromCurrentEnv(const ScriptVariableRef &dest) const {
	if (callstack.empty()) {
		return nullptr;
	}

	return callstack.back().resolveFromEnv(dest);
}

void ska::bytecode::Executor::release(const Operand& dest, ScriptExecution& current) {
	scriptFromOperand(dest, current).release(dest);
}

ska::NodeValue ska::bytecode::Executor::resolveValue(const Operand& variable, const ScriptExecution& current) {
	const auto* value = variable.type() == OperandType::VAR ? useFromCurrentEnv(variable.as<ScriptVariableRef>()) : nullptr;
	return value == nullptr || value->empty() ? scriptFromOperand(variable, current).getCell(variable) : *value;
}


ska::bytecode::ScriptExecution& ska::bytecode::Executor::scriptFromOperand(const Operand& v, ScriptExecution& current) {
	// An empty operand also holds a (default) ScriptVariableRef : it must not be routed to the script 0
	if (!v.empty() && std::holds_alternative<ScriptVariableRef>(v.content())) {
		const auto scriptIndex = v.as<ScriptVariableRef>().script;
		return scriptOrThrow(scriptIndex);
	}
	return current;
}

const ska::bytecode::ScriptExecution& ska::bytecode::Executor::scriptFromOperand(const Operand& v, const ScriptExecution& current) {
	// An empty operand also holds a (default) ScriptVariableRef : it must not be routed to the script 0
	if (!v.empty() && std::holds_alternative<ScriptVariableRef>(v.content())) {
		const auto scriptIndex = v.as<ScriptVariableRef>().script;
		return scriptOrThrow(scriptIndex);
	}
	return current;
}
