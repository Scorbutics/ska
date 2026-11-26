#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandPopInArr.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::POP_IN_ARR>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_IN_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto result = std::make_shared<NodeValueArrayRaw>();
	LOG_DEBUG << "Poping " << context.get<long>(left) << " elements from stack into a new array";
	context.pop(*result, context.get<long>(left));
	LOG_DEBUG << "Element poped : " << result;
	context.track(result);
	return result;
}
