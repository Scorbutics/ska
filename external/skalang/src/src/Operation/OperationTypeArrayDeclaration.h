#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::ARRAY_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto HasArrayType() {
			return node.size() > 0;
		}

    	inline auto GetArraySubType() {
			assert(HasArrayType());
        	return node[0].type().value();
    	}

		inline auto* GetArrayNode() {
			return HasArrayType() ? &node[0] : nullptr;
		}

		inline auto* GetArraySymbol() {
			return HasArrayType() ? node[0].symbol() : nullptr;
		}

		auto begin() const {
			return node.begin();
		}

		auto end() const  {
			return node.end();
		}
	};

	template<>
	class OperationType<Operator::ARRAY_TYPE_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto HasExplicitArrayType() const {
			return !node[1].logicalEmpty() && node[1].type().has_value();
		}

		inline auto GetArraySubType() const {
			assert(HasExplicitArrayType());
			return node[1].type().value();
		}

		inline auto* GetArraySymbol() {
			return node[0].symbol();
		}

		inline auto& GetArrayContent() const {
			return node[0];
		}
	};
}
