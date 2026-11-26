#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	
	template<>
	class OperationType<Operator::BLOCK> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		auto begin() {
			return node.begin();
		}

		auto end() {
			return node.end();
		}

		const auto& asNode() const { return node; }
	};
}
