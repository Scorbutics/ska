#include "InterpreterCommandEnd.h"
#include "Config/LoggerConfigLang.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::END>;


SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)


// END function offset [capturedCellsCount] : builds the function value (closure), once the function declaration is reached.
// The cells captured by the closure were pushed just before (PUSH_CELL), in the order of the function upvalues.
SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto function = NodeValue{ context.getRelativeInstruction(left.as<long>() + 1) };
	if (!right.empty()) {
		const auto capturedCells = right.as<long>();
		auto upvalues = std::make_shared<NodeValue::Upvalues>(static_cast<std::size_t>(capturedCells));
		for (auto index = capturedCells - 1; index >= 0; index--) {
			auto cellReference = NodeValue{};
			context.pop(cellReference);
			if (!cellReference.isCellReference()) {
				throw std::runtime_error("invalid bytecode : a closure can only capture cells");
			}
			(*upvalues)[static_cast<std::size_t>(index)] = cellReference.cell();
		}
		context.track(upvalues);
		function.setUpvalues(std::move(upvalues));
	}
	LOG_DEBUG << "End of function " << function;
	return function;
}
