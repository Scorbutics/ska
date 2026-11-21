#include "InterpreterCommandMapHas.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_HAS)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto map = context.get<NodeValueMap>(left);
	return map->find(*context.get<StringShared>(right)) != map->end();
}
