#include "InterpreterCommandJumpNif.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_NIF)(ExecutionContext& context, const Operand& left, const Operand& right) {
	if(!context.get<bool>(left)) {
	context.jumpRelative(context.get<long>(context.currentInstruction().dest()));
  }
	return {};
}
