#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class Symbol;
	
	template<>
	class OperationType<Operator::FUNCTION_CALL> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionType() {
			return node[0].type().value();
		}

		inline const auto& GetFunctionNameNode() {
			return node[0];
		}

		inline const auto* GetFunctionSymbol() {
			return node[0].symbol();
		}

		auto begin() const {
			return node.begin() + 1;
		}

		auto end() const {
			return node.end();
		}

		auto rbegin() const {
			return node.rbegin();
		}

		auto rend() const {
			return node.rend() - 1;
		}

		inline std::size_t GetFunctionParameterSize() const {
			return node.size() - 1;
		}

		inline const ASTNode& GetFunctionParameterValue(std::size_t index) const {
			return node[index + 1];
		}

	};
}
