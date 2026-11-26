#include "InterpreterCommandAddI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_I)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return context.get<long>(left) + context.get<long>(right);
}
