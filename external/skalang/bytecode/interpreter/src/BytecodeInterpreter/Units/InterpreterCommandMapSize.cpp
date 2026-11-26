#include "InterpreterCommandMapSize.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_SIZE)(ExecutionContext& context, const Operand& left, const Operand& right) {
	return static_cast<long>(context.get<NodeValueMap>(left)->size());
}
