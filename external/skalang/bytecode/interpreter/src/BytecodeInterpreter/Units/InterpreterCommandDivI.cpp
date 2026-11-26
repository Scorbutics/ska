#include "InterpreterCommandDivI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(DIV_I)(ExecutionContext& context, const Operand& left, const Operand& right) {
  const auto divisor = context.get<long>(right);
  if (divisor == 0) {
    throw std::runtime_error("division by zero");
  }
  return context.get<long>(left) / divisor;
}
