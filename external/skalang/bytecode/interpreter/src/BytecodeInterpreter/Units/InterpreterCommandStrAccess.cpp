#include "InterpreterCommandStrAccess.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(STR_ACCESS)(ExecutionContext& context, const Operand& left, const Operand& right) {
  const auto& value = *context.get<StringShared>(left);
  const auto index = context.get<long>(right);
  if (index < 0 || static_cast<std::size_t>(index) >= value.size()) {
    throw std::runtime_error("invalid string access at index " + std::to_string(index) + " on string size " + std::to_string(value.size()));
  }
  return std::make_shared<std::string>(1, value[static_cast<std::size_t>(index)]);
}
