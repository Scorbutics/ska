#pragma once

#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::FACTORY_PROTOTYPE_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunctionParametersList() {
			return node;
		}

		inline auto GetFunctionParametersSize() {
			return node.size() - 1;
		}

		inline auto GetFunctionReturnType() {
			return (*node.rbegin())->type().value();
		}

		inline auto begin() { return node.begin(); }
		inline auto end() { return node.end(); }

	};
}
