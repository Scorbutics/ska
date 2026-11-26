#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandPushFArr.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::PUSH_F_ARR>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_F_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  auto toPush = context.getCell(left);
	auto container = context.get<NodeValueArray>(right);
  LOG_DEBUG << "Pushing " << toPush.convertString() << " to array " << container;
  container->push_front(toPush);
	return container;
}
