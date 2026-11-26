#include "InterpreterCommandPopBArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_B_ARR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  auto numberToPop = context.get<long>(right);
	auto container = context.get<NodeValueArray>(left);

  if(numberToPop >= container->size()) {
	container->clear();
  } else {
	container->erase(container->end() - numberToPop, container->end());
  }

	return container;

}
