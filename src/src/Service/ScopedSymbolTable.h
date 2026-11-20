#pragma once
#include <optional>
#include <unordered_map>
#include <memory>
#include "NodeValue/Symbol.h"
#include "Base/Containers/order_indexed_string_map.h"
#include "SymbolFactory.h"

namespace ska {
	class SymbolTable;
	class ScriptAST;

	class ScopedSymbolTable :
		public SymbolFactory {
	using ChildrenScopedSymbolTable = std::vector<std::unique_ptr<ScopedSymbolTable>>;
	template<class T>
	using order_indexed_string_ref_map = insertion_indexed_map<std::string, T*>;

	public:
		ScopedSymbolTable(ScopedSymbolTable& parent, Symbol symbol) :
			m_parent(parent),
			m_symbol(std::move(symbol)) {
		}

		ScopedSymbolTable(ScopedSymbolTable& parent) :
			m_parent(parent) {
		}

		ScopedSymbolTable(std::string rootName);

		~ScopedSymbolTable() = default;

		ScopedSymbolTable& parent();
		const ScopedSymbolTable& parent() const;

		ScopedSymbolTable& createNested(std::optional<Symbol> symbol);
		ScopedSymbolTable& createNested(std::string name = "", const ScriptAST* script = nullptr, bool exported = false);
		// Gives a name (symbol) to an existing unnamed nested symbol table, keeping its position and its children
		ScopedSymbolTable& nameNested(std::size_t position, std::string name, bool exported = false);
		// Recursively re-registers exported children in their position order (used after an out of order rebuild)
		void rebuildExportsOrder();

		const ScopedSymbolTable* owner() const {
			return &m_parent == this || directOwner() != nullptr ? directOwner() : m_parent.owner();
		}

		const ScopedSymbolTable* directOwner() const { return m_parent.m_symbol.has_value() ? &m_parent : nullptr; }
		ScopedSymbolTable* directOwner() { return m_parent.m_symbol.has_value() ? &m_parent : nullptr; }

		bool changeTypeIfRequired(const Type& value);

		std::optional<std::size_t> id(const Symbol& field) const;

		const ScopedSymbolTable* operator[](const std::string& key) const;
		ScopedSymbolTable* operator[](const std::string& key);

		const ScopedSymbolTable* operator()(const std::string& key) const;
		ScopedSymbolTable* operator()(const std::string& key);

		const ScopedSymbolTable* operator[](std::size_t index) const;
		ScopedSymbolTable* operator[](std::size_t index);

		auto end() const { return m_children.end(); }
		auto begin() const { return m_children.begin(); }
		auto end() { return m_children.end(); }
		auto begin() { return m_children.begin(); }

		const ScopedSymbolTable* back() const { return m_children.empty() ? nullptr : &m_children.back(); }
		ScopedSymbolTable* back() { return m_children.empty() ? nullptr : &m_children.back(); }
		std::size_t size() const { return m_children.size(); }
		bool empty() const { return m_children.empty(); }

		const Symbol* symbol() const { return m_symbol.has_value() ? &m_symbol.value() : nullptr; }
		Symbol* symbol() { return m_symbol.has_value() ? &m_symbol.value() : nullptr; }

		void implement(ScopedSymbolTable& childInstanceSymbolTable);

		const ScopedSymbolTable* classTable() const { return m_classTable; }

		const std::string& name() const { return m_symbol.has_value() ? m_symbol.value().name() : EMPTY_STR; }
	private:
		static const std::string EMPTY_STR;
		Symbol& emplace(Symbol symbol);
		void ensureNotLocked() const;

		order_indexed_string_map<ScopedSymbolTable> m_children;
		order_indexed_string_ref_map<ScopedSymbolTable> m_exported;
		ScopedSymbolTable& m_parent = *this;
		ScopedSymbolTable* m_classTable = nullptr;
		std::optional<Symbol> m_symbol;
		bool m_locked = false;
	};
}
