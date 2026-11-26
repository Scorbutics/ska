#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class Symbol;

	template<>
	class OperationType<Operator::FIELD_ACCESS> {
	private:
		const ASTNode& node;

		inline auto& GetObject() {
			assert(node.size() > 0);
			return node[0];
		}

		inline auto& GetField() {
			assert(node.size() > 1);
			return node[1];
		}

	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto& GetObjectNameNode() {
			return GetObject();
		}

		inline const auto* GetObjectSymbolTable() {
			return GetObject().symbolTable();
		}

		inline auto& GetFieldNameNode() {
			return GetField();
		}

    	inline auto GetObjectType() {
        	return GetObject().type().value();
    	}

		inline auto GetFieldType() {
			const auto objectType = GetField().type().value();
			return objectType;
		}
	};
}
