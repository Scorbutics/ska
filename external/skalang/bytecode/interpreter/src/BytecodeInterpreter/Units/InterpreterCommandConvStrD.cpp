#include "InterpreterCommandConvStrD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_STR_D)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return std::stof(*context.get<StringShared>(left));
}
