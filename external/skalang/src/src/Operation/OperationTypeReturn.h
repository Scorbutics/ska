#pragma once

#include "OperationType.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::RETURN> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline const auto& GetValue() const {
			return node[0];
		}
	};

}