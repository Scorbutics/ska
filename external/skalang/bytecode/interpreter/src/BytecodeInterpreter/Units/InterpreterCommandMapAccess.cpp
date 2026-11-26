#include "InterpreterCommandMapAccess.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_ACCESS)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& key = *context.get<StringShared>(right);
	auto map = context.get<NodeValueMap>(left);
	const auto value = map->find(key);
	if (value == map->end()) {
		throw std::runtime_error("the map has no key \"" + key + "\" (check it with has(key))");
	}
	return value->second.resolved();
}
