#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::SCRIPT_LINK> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		auto& GetValue() {
			return node[0];
		}

		auto begin() {
			return GetValue().begin();
		}

		auto end() {
			return GetValue().end();
		}
	};
}
