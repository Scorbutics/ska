#include "InterpreterCommandPushArrArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_ARR_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto& leftSide = *context.get<NodeValueArray>(left);
	auto& rightSide = *context.get<NodeValueArray>(right);

	auto container = context.get<NodeValueArray>(left);
	container->insert(container->end(), rightSide.begin(), rightSide.end());
	return container;
}
