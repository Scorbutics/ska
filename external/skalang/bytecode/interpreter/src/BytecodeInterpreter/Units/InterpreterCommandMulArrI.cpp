#include "InterpreterCommandMulArrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_ARR_I)(ExecutionContext& context, const Operand& left, const Operand& right) {
  auto container = context.get<NodeValueArray>(left);
  if(container->empty()) {
	  return container;
  }

  auto count = context.get<long>(right);
  if(count < 0) {
	  throw std::runtime_error("cannot multiply an array a negative number of times");
  }

  if(count == 0) {
		container->clear();
  } else {
	  auto copy = *container;
	  for(long i = 0; i < count; i++) {
		  container->insert(container->end(), copy.begin(), copy.end());
	  }
  }
  return container;
}
