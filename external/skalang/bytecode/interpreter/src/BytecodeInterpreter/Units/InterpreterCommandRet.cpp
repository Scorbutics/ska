#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandRet.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::RET>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(RET)(ExecutionContext& context, const Operand& left, const Operand& right) {
	// The returned value is read in the frame of the returning call, before leaving it
	auto returnedValue = context.getCell(context.currentInstruction().dest());
	LOG_DEBUG << "Returning " << returnedValue.convertString();
	context.jumpReturn();
	context.push(std::move(returnedValue));
	// The frame of the call was released : a good time to collect reference cycles, from time to time
	context.collectCyclesIfNeeded();
	return {};
}
