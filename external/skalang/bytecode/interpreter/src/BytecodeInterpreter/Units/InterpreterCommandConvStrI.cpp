#include "InterpreterCommandConvStrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_STR_I)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return static_cast<long>(std::stoi(*context.get<StringShared>(left)));
}
