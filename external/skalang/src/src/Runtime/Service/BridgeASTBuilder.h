#pragma once
#include <functional>
#include <cassert>
#include <unordered_map>

#include "Base/Patterns/Observable.h"
#include "Base/Meta/TupleUtils.h"
#include "Container/sorted_observable.h"

#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"

#include "NodeValue/ASTNodePtr.h"
#include "Service/Matcher/MatcherType.h"
#include "BridgeFunction.h"

namespace ska {
	class SymbolTable;
	class TypeBuilder;
	class SymbolTableUpdater;
	struct ReservedKeywordsPool;
	class StatementParser;

	class BridgeASTBuilder;
	class BridgeASTBuilderSymbolTableLock {
	public:
		BridgeASTBuilderSymbolTableLock(BridgeASTBuilder& factory, SymbolTable& table);
		BridgeASTBuilderSymbolTableLock(BridgeASTBuilderSymbolTableLock&&) noexcept;
		BridgeASTBuilderSymbolTableLock(const BridgeASTBuilderSymbolTableLock&) = delete;
		BridgeASTBuilderSymbolTableLock& operator=(BridgeASTBuilderSymbolTableLock&&) = delete;
		BridgeASTBuilderSymbolTableLock& operator=(const BridgeASTBuilderSymbolTableLock&) = delete;

		~BridgeASTBuilderSymbolTableLock();
		void release();
	private:
		BridgeASTBuilder& m_factory;
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class BridgeASTBuilder :
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent>,
		public observable_priority_queue<ImportTokenEvent>,
		public observable_priority_queue<ReturnTokenEvent> {
		friend class BridgeASTBuilderSymbolTableLock;
	public:
		BridgeASTBuilder(TypeBuilder& typeBuilder, const ReservedKeywordsPool& reserved);
		virtual ~BridgeASTBuilder();

		ASTNodePtr makeFunction(ScriptAST& script, const BridgeFunction& data);
		std::vector<ASTNodePtr> makeFieldList(ScriptAST& script, const BridgeFunction& data);
		ASTNodePtr makeScript(ScriptAST& script, std::vector<ASTNodePtr> body);

	private:
		ASTNodePtr makeFunctionFactoryPrototype(ScriptAST& script, const Type& fullTypeFunction, const std::string& name);
		ASTNodePtr makeFunctionDeclaration(ScriptAST& script, ASTNodePtr prototype, const BridgeFunction& data);
		ASTNodePtr makeFactoryDeclaration(ScriptAST& script, ASTNodePtr prototype, const BridgeFunction& data);
		ASTNodePtr makeFunctionParameterOrReturnType(ScriptAST& script, ASTNodePtr nodeType, std::size_t parameterIndex, std::size_t totalParameters);
		ASTNodePtr makeFunctionName(ScriptAST& script, const std::string& name);
		ASTNodePtr makeFunctionPrototype(ScriptAST& script, ASTNodePtr nameNode, std::deque<ASTNodePtr> parametersAndReturn);
		ASTNodePtr makeFactoryPrototype(ScriptAST& script, ASTNodePtr nameNode, std::deque<ASTNodePtr> parametersAndReturn);
		ASTNodePtr makeVariable(ScriptAST& script, const std::string& name, ASTNodePtr value);
		ASTNodePtr makeFactoryEmptyBody() const;
		ASTNodePtr makeFactoryReturnObject(ScriptAST& script, std::vector<BridgeFunction> fieldList);
		std::deque<ASTNodePtr> makeFunctionInputOutput(ScriptAST& script, const Type& fullTypeFunction);

		void internalUnlisten(SymbolTable& symbolTable);
		void internalListen(SymbolTable& symbolTable);

		std::unordered_map<const SymbolTable*, int> m_symbolTableLockCounter;
		TypeBuilder& m_typeBuilder;
		const ReservedKeywordsPool& m_reserved;
		MatcherType m_matcherType;
	};
}
