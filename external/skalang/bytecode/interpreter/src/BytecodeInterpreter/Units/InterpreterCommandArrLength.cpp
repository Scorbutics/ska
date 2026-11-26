#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrLength.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_LENGTH>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_LENGTH>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_LENGTH>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_LENGTH)(ExecutionContext& context, const Operand& left, const Operand& right) {
    assert(IsMemoryOperand(left.type()));
    assert(right.type() == OperandType::EMPTY);

    LOG_DEBUG << "Computing array length of object \"" << context.getCell(left).convertString() << "\" of type " << left.type();

    auto array = context.get<NodeValueArray>(left);
    LOG_INFO << "[Cell has length " << array->size() << " (object " << left << ")]";

    return static_cast<long>(array->size());
}
