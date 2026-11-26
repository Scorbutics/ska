#include "InterpreterCommandMulIStr.h"
#include "InterpreterCommandMulStrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_I_STR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return InterpreterCommand<Command::MUL_STR_I>{m_interpreter, m_generator}.interpret(context, right, left);
}
