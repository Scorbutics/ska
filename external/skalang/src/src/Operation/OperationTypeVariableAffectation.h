#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::AFFECTATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetVariableNameNode() {
			return node[0];
		}
		
		inline const auto& GetVariableValueNode() {
			return node[1];
		}
	};
}
