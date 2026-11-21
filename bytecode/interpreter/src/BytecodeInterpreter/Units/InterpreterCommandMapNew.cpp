#include "InterpreterCommandMapNew.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_NEW)(ExecutionContext& context, const Operand& left, const Operand& right) {
	// A new empty map (registered in the cycle collector : its values can reference it)
	auto map = std::make_shared<NodeValueMap::element_type>();
	context.track(map);
	return NodeValue{ NodeValueMap{ std::move(map) } };
}
