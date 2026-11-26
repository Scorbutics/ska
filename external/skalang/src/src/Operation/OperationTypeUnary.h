#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::UNARY> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetValue() {
			return node.name();
		}
		
		inline const auto& asNode() {
			return node;
		}
	};
	
	template<>
	class OperationType<Operator::LITERAL> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetValue() {
			return node.name();
		}

		inline const auto& GetType() const {
			return node.type();
		}
		
		inline const auto& asNode() {
			return node;
		}
	};
}
