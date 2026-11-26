#include "InterpreterCommandJumpRel.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_REL)(ExecutionContext& context, const Operand& left, const Operand& right) {
	context.jumpRelative(context.get<long>(context.currentInstruction().dest()));
	return {};
}
