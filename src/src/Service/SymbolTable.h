#pragma once

#include "Container/sorted_observable.h"

#include "NodeValue/AST.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/FilterTokenEvent.h"

#include "ScopedSymbolTable.h"
#include "SymbolTableOperation.h"

namespace ska {

	class StatementParser;

	class SymbolTable;
	class ParserListenerLock {
	public:
		ParserListenerLock(SymbolTable& symbolTable, StatementParser& parser);
		ParserListenerLock(ParserListenerLock&&) noexcept;
		ParserListenerLock(const ParserListenerLock&) = delete;
		ParserListenerLock& operator=(ParserListenerLock&&) = delete;
		ParserListenerLock& operator=(const ParserListenerLock&) = delete;

		~ParserListenerLock();
		void release();
	private:
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	class SymbolTable :
    	public PriorityObserver<VarTokenEvent>,
   		public PriorityObserver<BlockTokenEvent>,
    	public PriorityObserver<FunctionTokenEvent>,
    	public PriorityObserver<ReturnTokenEvent>,
		public PriorityObserver<ImportTokenEvent>,
		public PriorityObserver<ScriptLinkTokenEvent>,
		public PriorityObserver<FilterTokenEvent>,
		public SymbolFactory {

    	using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class ParserListenerLock;
	public:
		SymbolTable(std::string name);
		SymbolTable(std::string name, StatementParser& parser);
		virtual ~SymbolTable();

		[[nodiscard]]
		ParserListenerLock listenParser(StatementParser& parser);

		ScopedSymbolTable* operator[](std::size_t index) { return (*m_currentTable)[index]; }
		const ScopedSymbolTable* operator[](std::size_t index) const { return (*m_currentTable)[index];  }

		ScopedSymbolTable* operator[](const std::string& key) { return (*m_currentTable)[key]; }
		const ScopedSymbolTable* operator[](const std::string& key) const { return (*m_currentTable)[key]; }

		ScopedSymbolTable* operator()(const std::string& key) { return (*m_currentTable)(key); }
		const ScopedSymbolTable* operator()(const std::string& key) const { return (*m_currentTable)(key); }

		const ScopedSymbolTable* enclosingType() const {
			return m_currentTable->owner();
		}

		const ScopedSymbolTable& current() const {
			return *m_currentTable;
		}

		std::size_t size() const {
			return m_currentTable->size();
		}

		bool changeTypeIfRequired(const Type& value);
		const ScopedSymbolTable* lookup(SymbolTableLookup strategy, SymbolTableNested depth = SymbolTableNested::current()) const;
		ScopedSymbolTable* lookup(SymbolTableLookup strategy, SymbolTableNested depth = SymbolTableNested::current());

		auto begin() { return m_rootTable->begin(); }
		auto end() { return m_rootTable->end(); }
		auto begin() const { return m_rootTable->begin(); }
		auto end() const  { return m_rootTable->end(); }

		ScopedSymbolTable& root() { return *m_rootTable; }
		const ScopedSymbolTable& root() const { return *m_rootTable; }

	private:
		bool match(VarTokenEvent&);
		bool nestedTable(const BlockTokenEvent&);
		bool matchFunction(FunctionTokenEvent&);
		bool matchReturn(const ReturnTokenEvent&);
		bool matchImport(const ImportTokenEvent&);
		bool matchScriptLink(const ScriptLinkTokenEvent&);
		bool matchFilter(FilterTokenEvent&);

		void internalListenParser(StatementParser& parser);
		void internalUnlistenParser();

		StatementParser* m_parser = nullptr;
    	std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
	};
}
