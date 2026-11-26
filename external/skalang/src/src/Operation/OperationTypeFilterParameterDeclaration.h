#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::FILTER_PARAMETER_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline Type GetCollectionType() const {
			assert(node.size() == 1);
			return node[0].type().value_or(Type{});
		}

		std::string GetIteratorName() const {
			return node.name();
		}

	};
}
