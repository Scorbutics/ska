#pragma once

#include "BytecodeInterpreter/InterpreterCommand.h"

namespace ska {
  namespace bytecode {
	SKALANG_BYTECODE_INTERPRETER_COMMAND_DEFINE(Command::ARR_HAS);
	// Equality of two values of the elements of an array (int, float, bool, string : the strings by their text)
	bool ValuesEqual(const NodeValue& left, const NodeValue& right);
  }
}
