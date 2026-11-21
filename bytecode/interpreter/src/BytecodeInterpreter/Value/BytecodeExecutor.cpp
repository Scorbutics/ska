#include "Config/LoggerConfigLang.h"
#include "BytecodeExecutor.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Executor);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Executor)

void ska::bytecode::Executor::enterScriptLevel() {
	auto frame = CallFrame{};
	frame.scriptLevel = true;
	frames.push_back(std::move(frame));
}

void ska::bytecode::Executor::leaveScriptLevel() {
	// Also leaves the call frames still opened above it (when an error interrupted the execution)
	while (!frames.empty()) {
		const auto isScriptLevel = frames.back().scriptLevel;
		frames.pop_back();
		if (isScriptLevel) {
			break;
		}
	}
}

void ska::bytecode::Executor::call(NodeValue function, ScriptVariableRef returnAddress) {
	auto frame = CallFrame{};
	frame.function = std::move(function);
	frame.returnAddress = std::move(returnAddress);
	frames.push_back(std::move(frame));
}

ska::ScriptVariableRef ska::bytecode::Executor::returnFromCall() {
	if (frames.empty() || frames.back().scriptLevel) {
		throw std::runtime_error("invalid bytecode : return outside of a function call");
	}
	auto returnAddress = frames.back().returnAddress;
	frames.pop_back();
	return returnAddress;
}

ska::bytecode::CallFrame& ska::bytecode::Executor::frame() {
	if (frames.empty()) {
		throw std::runtime_error("invalid bytecode : no current call frame");
	}
	return frames.back();
}

const ska::bytecode::CallFrame& ska::bytecode::Executor::frame() const {
	if (frames.empty()) {
		throw std::runtime_error("invalid bytecode : no current call frame");
	}
	return frames.back();
}

const ska::NodeValue::Cell& ska::bytecode::Executor::upvalue(std::size_t upvalueIndex) const {
	const auto* upvalues = frame().function.upvalues();
	if (upvalues == nullptr || upvalueIndex >= upvalues->size()) {
		throw std::runtime_error("invalid bytecode : no captured cell at index " + std::to_string(upvalueIndex));
	}
	return (*upvalues)[upvalueIndex];
}

ska::NodeValue ska::bytecode::Executor::resolveValue(const Operand& operand, const ScriptExecution& current) {
	switch (operand.type()) {
	case OperandType::EMPTY:
		return {};
	case OperandType::LOCAL: {
		const auto* value = frame().locals.get_if(index(operand));
		return value == nullptr ? NodeValue{} : value->resolved();
	}
	case OperandType::REG: {
		const auto* value = frame().registers.get_if(index(operand));
		return value == nullptr ? NodeValue{} : value->resolved();
	}
	case OperandType::UPVAL:
		return static_cast<const NodeValue&>(*upvalue(index(operand)));
	case OperandType::SELF:
		return frame().function;
	default:
		return scriptFromOperand(operand, current).getCell(operand).resolved();
	}
}

void ska::bytecode::Executor::makeCell(const Operand& local) {
	if (local.type() != OperandType::LOCAL) {
		throw std::runtime_error("invalid bytecode : only a local variable can be made a cell");
	}
	frame().locals.replace(index(local), NodeValue::CellReference(std::make_shared<NodeValue>()));
}

ska::NodeValue ska::bytecode::Executor::cellReference(const Operand& operand) const {
	switch (operand.type()) {
	case OperandType::LOCAL: {
		const auto* value = frame().locals.get_if(index(operand));
		if (value == nullptr || !value->isCellReference()) {
			throw std::runtime_error("invalid bytecode : the captured local variable " + std::to_string(index(operand)) + " is not a cell");
		}
		return *value;
	}
	case OperandType::UPVAL:
		return NodeValue::CellReference(upvalue(index(operand)));
	case OperandType::SELF:
		// A nested function using the name of the running function : it captures a cell holding the running function value
		// (the running function itself does not capture anything, so there is no reference cycle)
		return NodeValue::CellReference(std::make_shared<NodeValue>(frame().function));
	default:
		throw std::runtime_error("invalid bytecode : only a local variable or an upvalue can be captured");
	}
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
