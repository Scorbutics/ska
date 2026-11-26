#include "InterpreterCommandMapSet.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_SET)(ExecutionContext& context, const Operand& left, const Operand& right) {
	// MAP_SET map key value
	const auto& mapOperand = context.currentInstruction().dest();
	auto map = context.get<NodeValueMap>(mapOperand);
	(*map)[*context.get<StringShared>(left)] = context.getCell(right).resolved();
	return {};
}
