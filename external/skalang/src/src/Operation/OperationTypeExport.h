#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::EXPORT> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetVariable() {
			assert(node.size() == 1);
			return node[0];
		}
		

	};
}
