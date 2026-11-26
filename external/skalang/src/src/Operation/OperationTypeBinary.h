#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::BINARY> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}
		
		inline auto GetFirstType() {
			return node[0].type().value();
		}
	
		inline auto GetSecondType() {
			return node[1].type().value();
		}

		inline const auto& GetFirstNode() {
			return node[0];
		}

		inline const auto& GetSecondNode() {
			return node[1];
		}

		inline auto& asNode() {
			return node;
		}

		inline auto GetOperator() {
			return node.name();
		}
	};
}
