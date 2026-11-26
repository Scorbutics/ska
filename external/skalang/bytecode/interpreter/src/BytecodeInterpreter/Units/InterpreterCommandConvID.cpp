#include "InterpreterCommandConvID.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_I_D)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return static_cast<double>(context.get<long>(left));
}
