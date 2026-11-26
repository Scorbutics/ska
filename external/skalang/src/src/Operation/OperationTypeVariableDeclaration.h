#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {

	template<>
	class OperationType<Operator::DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetVariableName() const {
			assert(!node.name().empty());
			return node.name();
		}

		inline auto& GetVariableValueNode() const {
			assert(node.size() != 0);
			return node[0];
		}

		inline const auto& GetVariableNameNode() const {
			return node;
		}
	};
}
