#include "InterpreterCommandArrHas.h"

bool ska::bytecode::ValuesEqual(const NodeValue& left, const NodeValue& right) {
	if (left.is<StringShared>() && right.is<StringShared>()) {
		return *left.nodeval<StringShared>() == *right.nodeval<StringShared>();
	}
	return left.resolved() == right.resolved();
}

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_HAS)(ExecutionContext& context, const Operand& left, const Operand& right) {
	// ARR_HAS dest array value
	auto array = context.get<NodeValueArray>(left);
	const auto value = context.getCell(right).resolved();
	for (const auto& element : *array) {
		if (ValuesEqual(element, value)) {
			return true;
		}
	}
	return false;
}
