#pragma once
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::MAP_LITERAL> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const ASTNode& asNode() const { return node; }
		inline std::size_t size() const { return node.size(); }
		inline const ASTNode& operator[](std::size_t index) const { return node[index]; }
	};
}
