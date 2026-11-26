#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandPush.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::PUSH>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto un = context.getCell(context.currentInstruction().dest());
	auto dos = context.getCell(left);
	auto tres = context.getCell(right);

	LOG_DEBUG << "Pushing elements " << un.convertString() << ", " << dos.convertString() << " and " << tres.convertString() << " into the stack";

	context.push(un, dos, tres);
	return {};
}
