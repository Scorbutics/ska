#pragma once
#include <utility>
#include "Type.h"
#include "Symbol.h"

namespace ska {
	class ASTNode;
	class ScopedSymbolTable;
	class SymbolTable;
	struct TypeHierarchy {
		TypeHierarchy(Type t);
		TypeHierarchy(Type t, const ASTNode* symbolNode);
		TypeHierarchy(const SymbolTable& table);
		TypeHierarchy(Type t, const SymbolTable& table);
		TypeHierarchy(const ScopedSymbolTable& table);
		TypeHierarchy(Type t, const ScopedSymbolTable& table);

		const ScopedSymbolTable* link() { return m_link; }

	private:
		const ScopedSymbolTable* m_link = nullptr;
	public:
		Type type;
	};
}
