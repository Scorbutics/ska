#include "InterpreterCommandCmpBool.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CMP_BOOL)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return (context.get<bool>(left) == context.get<bool>(right)) ? 0l : 1l;
}
