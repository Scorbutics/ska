#pragma once
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	// [:]: T : an empty map of values of the type T (node : the type T)
	template<>
	class OperationType<Operator::MAP_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const Type& GetMapSubType() const {
			return node[0].type().value();
		}
	};
}
