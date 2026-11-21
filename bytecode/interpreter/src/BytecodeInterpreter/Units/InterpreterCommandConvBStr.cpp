#include "InterpreterCommandConvBStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_B_STR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return std::make_shared<std::string>(context.get<bool>(left) ? "true" : "false");
}
