#pragma once
#include <memory>
#include <tuple>
#include <deque>
#include <ostream>

namespace ska {
	class NodeValue;
	using NodeValueArrayRaw = std::deque<NodeValue>;
	using NodeValueArray = std::shared_ptr<NodeValueArrayRaw>;

	std::ostream& operator<<(std::ostream& stream, const NodeValueArray& nodeArray);
}
