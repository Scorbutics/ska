#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {

	template<>
	class OperationType<Operator::FUNCTION_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunction() {
			return node;
		}

		inline auto& GetFunctionPrototype() {
			return node[0];
		}

		inline auto& GetFunctionBody() {
			return node[1];
		}
	};

	template<>
	class OperationType<Operator::FUNCTION_PROTOTYPE_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto GetParameterSize() {
			return node.size() - 1;
		}

		inline auto& GetFunction() {
			return node;
		}

		auto begin() const {
			return node.begin();
		}

		auto end() const {
			return node.end();
		}

		auto rbegin() const {
			return node.rbegin();
		}

		auto rend() const {
			return node.rend();
		}

	};
}
