#include "InterpreterCommandAddEnv.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_ENV)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& variable = context.currentInstruction().dest();
	assert(variable.type() == OperandType::VAR || variable.type() == OperandType::REG);

	auto addedVariable = context.getCell(variable);
	context.pushInEnv(variable.as<ScriptVariableRef>(), std::move(addedVariable));

	// Optional operands : [ADD_ENV|variable|object|fieldIndex]
	// The captured variable is also a field of the returned object : both must share the same storage.
	if (left.type() != OperandType::EMPTY) {
		auto object = context.get<NodeValueArray>(left);
		const auto index = context.get<long>(right);
		if (index < 0 || static_cast<std::size_t>(index) >= object->size()) {
			throw std::runtime_error("invalid object field index " + std::to_string(index) + " while aliasing env variable");
		}
		context.aliasEnvInCell(variable.as<ScriptVariableRef>(), (*object)[index]);
	}
	return {};
}
