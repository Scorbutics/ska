#include "InterpreterCommandStrLength.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(STR_LENGTH)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return static_cast<long>(context.get<StringShared>(left)->size());
}
