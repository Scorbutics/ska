#include "InterpreterCommandCell.h"

// CELL local : the local variable is captured by a closure, so it lives in a cell (shared with the closures capturing it).
// Every access to the local variable then goes through the cell.
SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CELL)(ExecutionContext& context, const Operand& left, const Operand& right) {
	context.makeCell(context.currentInstruction().dest());
	return {};
}
