#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::USER_DEFINED_OBJECT> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		auto empty() {
			return node.size() == 0;
		}

		auto begin() {
			return node.begin();
		}

		auto end() {
			return node.end();
		}
	};
}
