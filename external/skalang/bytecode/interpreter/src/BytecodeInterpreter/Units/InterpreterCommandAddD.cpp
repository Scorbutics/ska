#include "InterpreterCommandAddD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_D)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return context.get<double>(left) + context.get<double>(right);
}
