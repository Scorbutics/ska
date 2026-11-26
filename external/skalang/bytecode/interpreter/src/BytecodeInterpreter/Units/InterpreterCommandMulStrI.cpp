#include "InterpreterCommandMulStrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_STR_I)(ExecutionContext& context, const Operand& left, const Operand& right) {
  auto container = context.get<StringShared>(left);
  if(container->empty()) {
	  return container;
  }

  auto count = context.get<long>(right);
  if(count < 0) {
	  throw std::runtime_error("cannot multiply a string a negative number of times");
  }

  if(count == 0) {
		return std::make_shared<std::string>();
  } else {
	  auto ss = std::stringstream {};
	  for(long i = 0; i < count; i++) {
		  ss << *container;
	  }
	  container = std::make_shared<std::string>(ss.str());
  }
  return container;
}
