#include "Service/ScopedSymbolTable.h"
#include "Service/SymbolTable.h"
#include "TypeHierarchy.h"
#include "NodeValue/AST.h"

ska::TypeHierarchy::TypeHierarchy(Type t):
	m_link(t.m_symbolTable),
	type(std::move(t)) {
}

ska::TypeHierarchy::TypeHierarchy(Type t, const ASTNode* symbolNode):
	m_link(symbolNode != nullptr ? symbolNode->symbolTable(): nullptr),
	type(std::move(t)) {

}

ska::TypeHierarchy::TypeHierarchy(Type t, const SymbolTable& table):
	m_link(&table.current()),
	type(std::move(t)) {
}

ska::TypeHierarchy::TypeHierarchy(const SymbolTable& table):
	m_link(&table.current()),
	type(m_link->symbol() ? m_link->symbol()->type() : Type{}) {
}

ska::TypeHierarchy::TypeHierarchy(const ScopedSymbolTable& table):
	m_link(&table),
	type(m_link->symbol() ? m_link->symbol()->type() : Type{}) {
}

ska::TypeHierarchy::TypeHierarchy(Type t, const ScopedSymbolTable& table) :
	m_link(&table),
	type(std::move(t)) {
}
