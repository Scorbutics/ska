#include "InterpreterCommandMov.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV)(ExecutionContext& context, const Operand& left, const Operand& right) {
  // Moves a value, never a reference : "a = obj.field" must copy the field value, not alias the field.
  // Writing through a reference (ex : "obj.field = a") still works, as the destination itself is then the reference.
  return context.getCell(left).resolved();
}
