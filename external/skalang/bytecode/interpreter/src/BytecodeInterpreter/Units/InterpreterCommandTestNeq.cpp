#include "InterpreterCommandTestNeq.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(TEST_NEQ)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return context.get<long>(left) != 0;
}
