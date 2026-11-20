#include "InterpreterCommandPushCell.h"

// PUSH_CELL local|upval : pushes the cell of a captured variable (not its value),
// to build a closure (END) or an object field aliasing the variable (POP_IN_VAR).
SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_CELL)(ExecutionContext& context, const Operand& left, const Operand& right) {
	context.pushCell(context.cellReference(context.currentInstruction().dest()));
	return {};
}
