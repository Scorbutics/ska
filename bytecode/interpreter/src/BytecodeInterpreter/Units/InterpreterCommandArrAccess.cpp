#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrAccess.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_ACCESS)(ExecutionContext& context, const Operand& left, const Operand& right) {
  assert(left.type() == OperandType::VAR || left.type() == OperandType::REG);
  assert(right.type() != OperandType::EMPTY && right.type() != OperandType::BIND_NATIVE && right.type() != OperandType::BIND_SCRIPT);

  LOG_DEBUG << "Index cell node " << right << " of type " << right.type() << " of object \"" << context.getCell(left).convertString() << "\" of type " << left.type();

  const auto index = context.get<long>(right);
  auto array = context.get<NodeValueArray>(left);
  LOG_INFO << "[Accessing cell at index " << index << "/" << array->size() << " of object " << left << "]";

  if(index >= array->size()) {
		throw std::runtime_error("invalid array access at index " + std::to_string(index) + " on array size " + std::to_string(array->size()));
  }
  auto& result = (*array)[index];
  LOG_INFO << "[Cell content : " << result.convertString() << "]";
	// TODO make env "copy" (it's not a copy as it's a shared_ptr) automatic when NodeValue is a recursive container (NodeValueArray, NodeValueMap, NodeValue*)
	// directly inside NodeValue class
	// An element that already owns an env (ex : an object instance stored in an array) keeps it :
	// only elements without env (ex : methods of an object) inherit the env of the accessed container.
	if (!result.hasEnv()) {
		result = NodeValue { context.getCell(left), result };
	}
	// Return a reference to the cell (not a copy) so that a field can be used as an lvalue :
	// "obj.field = value" generates ARR_ACCESS then MOV on the resulting register, which must write through into the object.
	// The env is also attached to the reference itself, because env users (e.g. jumpAbsolute's snapshot) read it without dereferencing.
	return NodeValue { result, &result };
}
