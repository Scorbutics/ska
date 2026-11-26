#pragma once

#include "OperationType.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::IF_ELSE> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetCondition() const {
			return node[0];
		}

		inline const auto& GetIfStatement() const {
			return node[1];
		}

		inline const auto& GetElseStatement() const {
			return node[2];
		}
	};

	template<>
	class OperationType<Operator::IF> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetCondition() const {
			return node[0];
		}

		inline const auto& GetIfStatement() const {
			return node[1];
		}
	};
}