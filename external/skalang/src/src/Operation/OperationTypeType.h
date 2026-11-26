#pragma once
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class ScopedSymbolTable;
	class SymbolTable;
	class ASTNode;

	template<>
	class OperationType<Operator::TYPE> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) { assert(node.size() == 3); }

		const ScopedSymbolTable* GetTypeSymbol(const SymbolTable& symbolTable) const;

		bool IsBuiltIn() const;
		bool IsObject() const;
		std::string GetName() const;
		std::string GetTypeName() const;
		const Type& GetType() const;

		// An array (T[]) or a map (T[:])
		bool IsArray() const  {
			return node.size() > 2 && !node[2].logicalEmpty();
		}

		bool IsMap() const {
			return IsArray() && node[2].name() == ":";
		}
	};
}
