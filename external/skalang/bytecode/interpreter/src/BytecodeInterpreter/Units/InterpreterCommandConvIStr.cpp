#include "InterpreterCommandConvIStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_I_STR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return std::make_shared<std::string>(std::to_string(context.get<long>(left)));
}
