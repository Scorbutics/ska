#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::SCRIPT_OBJECT> {
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
	};
}
