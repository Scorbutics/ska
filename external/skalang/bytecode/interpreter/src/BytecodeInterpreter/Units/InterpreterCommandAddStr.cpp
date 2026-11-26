#include "InterpreterCommandAddStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_STR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return std::make_shared<std::string>(*context.get<StringShared>(left) + *context.get<StringShared>(right));
}
