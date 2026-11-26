#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandSubArr.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::SUB_ARR>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SUB_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  auto leftSide = *context.get<NodeValueArray>(left);
	auto rightSide = *context.get<NodeValueArray>(right);

  if(rightSide.empty()) {
	return context.get<NodeValueArray>(left);
  }

  if(rightSide.size() > 2) {
	throw std::runtime_error("invalid array range deletion with an array size of " + std::to_string(rightSide.size()));
  }

  const auto firstVal = rightSide[0].nodeval<long>();
  if(firstVal < 0) {
	throw std::runtime_error("invalid array range deletion : the first value is negative");
  }

  long lastVal;
  if(rightSide.size() == 2) {
	lastVal = rightSide[1].nodeval<long>();
	if(lastVal < 0) {
  	throw std::runtime_error("invalid array range deletion : the last value is negative");
	} else if(firstVal > lastVal) {
  	throw std::runtime_error("invalid array range deletion : the first array value is greater than the last.");
	}
  } else {
	lastVal = firstVal;
  }

  LOG_DEBUG << "Range to delete : from " << firstVal << " to " << lastVal << " on an array of " << leftSide.size() << " elements";

  auto container = std::make_shared<NodeValueArrayRaw>();
  for(std::size_t i = 0; i < leftSide.size(); i++) {
	if (i < firstVal || i > lastVal) {
  	container->push_back(leftSide[i]);
	}
  }
	context.track(container);
	return container;

}
