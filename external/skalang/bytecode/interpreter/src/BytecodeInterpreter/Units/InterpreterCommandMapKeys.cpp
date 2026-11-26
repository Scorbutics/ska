#include "InterpreterCommandMapKeys.h"
#include <algorithm>
#include <string>
#include <vector>

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MAP_KEYS)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto map = context.get<NodeValueMap>(left);
	auto keys = std::vector<std::string>{};
	keys.reserve(map->size());
	for (const auto& entry : *map) {
		keys.push_back(entry.first);
	}
	// Sorted : the same order at each run
	std::sort(keys.begin(), keys.end());
	auto result = std::make_shared<NodeValueArrayRaw>();
	for (auto& key : keys) {
		result->push_back(NodeValue{ std::make_shared<std::string>(std::move(key)) });
	}
	context.track(result);
	return NodeValue{ NodeValueArray{ std::move(result) } };
}
