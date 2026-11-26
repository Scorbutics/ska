#pragma once

#include "OperationType.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::FOR_LOOP> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetCondition() const {
			return node[1];
		}

		inline const auto& GetIncrement() const {
			return node[2];
		}
		
		inline const auto& GetInitialization() const {
			return node[0];
		}
		
		inline const auto& GetStatement() const {
			return node[3];
		}
	};
}