#include "InterpreterCommandMulIArr.h"
#include "InterpreterCommandMulArrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_I_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return InterpreterCommand<Command::MUL_ARR_I>{m_interpreter, m_generator}.interpret(context, right, left);
}
