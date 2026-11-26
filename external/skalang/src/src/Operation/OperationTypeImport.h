#pragma once
#include <cassert>

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::IMPORT> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetScriptPathNode() const {
			return node[0];
		}
	};
}
