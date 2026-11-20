#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrSet.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_SET>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_SET>)

// ARR_SET array index value : stores the value in the array cell (or object field).
// If the cell is a captured variable cell (field aliasing a variable captured by a closure), the value is written into it.
SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_SET)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& arrayOperand = context.currentInstruction().dest();
	assert(IsMemoryOperand(arrayOperand.type()));

	const auto index = context.get<long>(left);
	auto array = context.get<NodeValueArray>(arrayOperand);
	if (index < 0 || static_cast<std::size_t>(index) >= array->size()) {
		throw std::runtime_error("invalid array access at index " + std::to_string(index) + " on array size " + std::to_string(array->size()));
	}

	LOG_INFO << "[Setting cell at index " << index << "/" << array->size() << " of object " << arrayOperand << "]";
	(*array)[index] = context.getCell(right);
	return {};
}
