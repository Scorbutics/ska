#include "InterpreterCommandPop.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP)(ExecutionContext& context, const Operand& left, const Operand& right) {
	assert(!context.currentInstruction().dest().empty());

	auto containerDest = NodeValue{};
	context.pop(containerDest);
	context.set(context.currentInstruction().dest(), std::move(containerDest));

	if (!left.empty()) {
		auto containerLeft = NodeValue{};
		context.pop(containerLeft);
		context.set(left, std::move(containerLeft));
	}

	if (!right.empty()) {
		auto containerRight = NodeValue{};
		context.pop(containerRight);
		context.set(right, std::move(containerRight));
	}

	return {};
}
