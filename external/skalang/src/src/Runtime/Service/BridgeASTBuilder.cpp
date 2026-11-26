#include <functional>
#include <fstream>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Config/LoggerConfigLang.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/ASTFactory.h"
#include "BridgeASTBuilder.h"
#include "NodeValue/ScriptAST.h"
#include "Service/ScriptNameBuilder.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::BridgeASTBuilder)

ska::BridgeASTBuilder::BridgeASTBuilder(TypeBuilder& typeBuilder, const ReservedKeywordsPool& reserved) :
	m_reserved(reserved),
	m_typeBuilder(typeBuilder),
	m_matcherType(reserved) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ReturnTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(m_typeBuilder);
}

void ska::BridgeASTBuilder::internalListen(SymbolTable& symbolTable) {
	auto [ element, isInserted ] = m_symbolTableLockCounter.emplace(&symbolTable, 0);
	auto& counter = m_symbolTableLockCounter.at(&symbolTable);
	counter++;
	SLOG(LogLevel::Debug) << "Lock counter " << counter;
	if (counter == 1) {
		SLOG(LogLevel::Info) << "Registering symbol table " << &symbolTable;
		observable_priority_queue<VarTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<ReturnTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<FunctionTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<BlockTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<ScriptLinkTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<ImportTokenEvent>::addObserver(symbolTable);
	}
}

void ska::BridgeASTBuilder::internalUnlisten(SymbolTable& symbolTable) {
	auto& counter = m_symbolTableLockCounter.at(&symbolTable);
	counter--;
	SLOG(LogLevel::Debug) << "Lock counter " << counter;
	if (counter <= 0) {
		SLOG(LogLevel::Info) << "Releasing symbol table " << &symbolTable;
		observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<BlockTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<FunctionTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<ReturnTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<VarTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<ImportTokenEvent>::removeObserver(symbolTable);
	}
}

ska::BridgeASTBuilder::~BridgeASTBuilder() {
	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<ReturnTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionParameterOrReturnType(ScriptAST& script, ASTNodePtr nodeType, std::size_t parameterIndex, std::size_t totalParameters) {
	if (totalParameters == parameterIndex) {
		SLOG(LogLevel::Info) << "Making function return type";
		return nodeType;
	}

	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	SLOG(LogLevel::Info) << "Making function parameter number " << parameterIndex;

	auto ss = std::stringstream{};
	ss << parameterIndex;
	auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER, Cursor{ parameterIndex, static_cast<ColumnIndex>(parameterIndex), static_cast<LineIndex>(1) } },
				std::move(nodeType));
	auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	SLOG(LogLevel::Debug) << " Deduced." ;
	return parameter;
}

std::deque<ska::ASTNodePtr> ska::BridgeASTBuilder::makeFunctionInputOutput(ScriptAST& script, const Type& fullTypeFunction) {
	auto parametersAndReturn = std::deque<ASTNodePtr>{};
	SLOG(LogLevel::Info) << " 4 - Making function parameters and return type";
	for (std::size_t index = 0u; index < fullTypeFunction.size(); index++) {
		const auto& type = fullTypeFunction[index];
		auto isReturnType = index == fullTypeFunction.size() - 1;
		SLOG(LogLevel::Info) << (isReturnType ? "return" : "parameter") << " : " << type;
		ASTNodePtr typeNode;
		if (type == ExpressionType::OBJECT && type.name() == fullTypeFunction.name()) {
			typeNode = ASTFactory::MakeLogicalNode(m_reserved.pattern<TokenGrammar::VARIABLE>());
		} else {
			typeNode = m_matcherType.match(type);
		}

		SLOG(LogLevel::Debug) << " Deduced '" << *typeNode << "'";
		parametersAndReturn.push_back(std::move(makeFunctionParameterOrReturnType(script, std::move(typeNode), index, fullTypeFunction.size() - 1)));
	}
	return parametersAndReturn;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionName(ScriptAST& script, const std::string& name) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	SLOG(LogLevel::Info) << " 3 - Making function name \"" << name << "\"";
	auto functionNameNode = ASTFactory::MakeLogicalNode(Token{name, TokenType::IDENTIFIER, {}});
	auto event = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, name };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionNameNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionPrototype(ScriptAST& script, ASTNodePtr nameNode, std::deque<ASTNodePtr> parametersAndReturn) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	SLOG(LogLevel::Info) << " 5 - Making function prototype (not a factory) \"" << nameNode->name() << "\"";
	auto functionPrototype = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(Token { nameNode->name(), nameNode->tokenType(), {} }, std::move(parametersAndReturn));
	auto event = VarTokenEvent::MakeFunction(*functionPrototype, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return functionPrototype;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeVariable(ScriptAST& script, const std::string& name, ASTNodePtr value) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	SLOG(LogLevel::Info) << " 8 - Making variable \"" << name << "\"";
	auto tokenField = Token {name, TokenType::IDENTIFIER, {} };
	auto variable = ASTFactory::MakeNode<Operator::DECLARATION>(std::move(tokenField), std::move(value));
	auto event = VarTokenEvent::template Make<VarTokenEventType::DECLARATION> (*variable, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return variable;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFactoryReturnObject(ScriptAST& script, std::vector<BridgeFunction> fieldList) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	auto returnStartEvent = ReturnTokenEvent { script };
	observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnStartEvent);

	auto returnFieldNodes = std::vector<ASTNodePtr>{};

	SLOG(LogLevel::Info) << " 7 - Build function field list";
	for (auto& field : fieldList) {
		auto fieldVariable = makeFunction(script, std::move(field));
		SLOG(LogLevel::Debug) << " Field built " << fieldVariable->name();
		returnFieldNodes.push_back(std::move(fieldVariable));
	}

	auto returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));
	auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*returnNode, script);
	observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
	return returnNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFactoryEmptyBody() const {
	return ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>());
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionDeclaration(ScriptAST& script, ASTNodePtr prototype, const BridgeFunction& data) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	const std::string& functionName = prototype->name();
	SLOG(LogLevel::Info) << " 6 - Making function declaration " << functionName;

	auto bodyNode = ASTNodePtr {};
	auto fieldList = data.makeFunctions();
	if (!fieldList.empty()) {
		throw std::runtime_error("No function factory type specified in script \"" + script.name() + "\" despite trying to bind C++ field-functions");
	} else {
		bodyNode = ASTFactory::MakeNode<Operator::BLOCK>();
	}
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER, {} };
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(prototype), std::move(bodyNode));

	auto event = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	SLOG(LogLevel::Info) << " Function building finished \"" << *functionDeclarationNode << "\"";
	return functionDeclarationNode;
}


ska::ASTNodePtr ska::BridgeASTBuilder::makeFactoryDeclaration(ScriptAST& script, ASTNodePtr prototype, const BridgeFunction& data) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	const auto functionName = Token{ prototype->name(), TokenType::IDENTIFIER, prototype->positionInScript() };

	auto fieldList = data.makeFunctions();
	auto bodyNode = ASTNodePtr{};
	if (!fieldList.empty()) {
		bodyNode = ASTFactory::MakeNode<Operator::BLOCK>(makeFactoryReturnObject(script, std::move(fieldList)));
	} else {
		bodyNode = makeFactoryEmptyBody();
	}

	SLOG(ska::LogLevel::Debug) << "factory declaration node";
	auto factoryDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototype), std::move(bodyNode));

	auto statementEvent = FunctionTokenEvent{ *factoryDeclarationNode, FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT, script, functionName.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	SLOG(LogLevel::Info) << " Factory building finished \"" << *factoryDeclarationNode << "\"";
	return factoryDeclarationNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFactoryPrototype(ScriptAST& script, ASTNodePtr nameNode, std::deque<ASTNodePtr> parameters) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	SLOG(LogLevel::Info) << " 5b - Making factory prototype \"" << nameNode->name() << "\"";

	auto functionName = Token{ nameNode->name(), TokenType::IDENTIFIER, nameNode->positionInScript() };

	auto index = std::size_t{ 0 };
	for (auto& parameter : parameters) {
		if (index != parameters.size() - 1) {
			auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], script);
			observable_priority_queue<VarTokenEvent>::notifyObservers(event);
		}
		index++;
	}
	auto factoryPrototypeNode = ASTFactory::MakeNode<Operator::FACTORY_PROTOTYPE_DECLARATION>(functionName, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*factoryPrototypeNode, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);


	return factoryPrototypeNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionFactoryPrototype(ScriptAST& script, const Type& fullTypeFunction, const std::string& name) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	SLOG(LogLevel::Info) << " 2 - Making function/factory prototype \"" << fullTypeFunction << "\"";

	if(fullTypeFunction.type() != ExpressionType::FUNCTION) { std::stringstream ss; ss << "type is not a function : " << name; throw std::runtime_error(ss.str()); };

	auto functionNameNode = makeFunctionName(script, name);
	auto parametersAndReturn = makeFunctionInputOutput(script, fullTypeFunction);
	const auto& returnTypeNode = parametersAndReturn.back();
	if (returnTypeNode->name() == m_reserved.pattern<TokenGrammar::VARIABLE>().name()) {
		return makeFactoryPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
	}

	return makeFunctionPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunction(ScriptAST& script, const BridgeFunction& data) {
	SLOG(LogLevel::Info) << " 1 - Making function/factory \"" << data.name() << "\"";
	auto prototype = makeFunctionFactoryPrototype(script, data.symbolTable().symbol()->type(), data.name());

	auto functionDeclaration = ASTNodePtr{};
	if (prototype->op() == Operator::FACTORY_PROTOTYPE_DECLARATION) {
		functionDeclaration = makeFactoryDeclaration(script, std::move(prototype), data);
	} else {
		functionDeclaration = makeFunctionDeclaration(script, std::move(prototype), data);
	}
	return makeVariable(script, data.name(), std::move(functionDeclaration));
}

std::vector<ska::ASTNodePtr> ska::BridgeASTBuilder::makeFieldList(ScriptAST& script, const BridgeFunction& data) {
	auto fieldList = data.makeFunctions();
	SLOG(LogLevel::Info) << "1a - Build fields list";
	auto scriptNodes = std::vector<ASTNodePtr>{};
	for (auto& field : fieldList) {
		auto fieldVariable = makeFunction(script, std::move(field));
		SLOG(LogLevel::Debug) << " Field built " << fieldVariable->name();
		scriptNodes.push_back(std::move(fieldVariable));
	}
	return scriptNodes;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeScript(ScriptAST& script, std::vector<ASTNodePtr> body) {
	auto result = ASTFactory::MakeNode<Operator::SCRIPT_OBJECT>(std::move(body));
	auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*result, script);
	observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
	return result;
}

ska::BridgeASTBuilderSymbolTableLock::BridgeASTBuilderSymbolTableLock(BridgeASTBuilder& factory, SymbolTable& table) :
	m_factory(factory),
	m_symbolTable(table) {
	m_factory.internalListen(m_symbolTable);
}

ska::BridgeASTBuilderSymbolTableLock::BridgeASTBuilderSymbolTableLock(BridgeASTBuilderSymbolTableLock&& factoryLock) noexcept :
	m_factory(factoryLock.m_factory),
	m_symbolTable(factoryLock.m_symbolTable),
	m_freed(factoryLock.m_freed){
	factoryLock.m_freed = true;
}

ska::BridgeASTBuilderSymbolTableLock::~BridgeASTBuilderSymbolTableLock() {
	release();
}

void ska::BridgeASTBuilderSymbolTableLock::release() {
	if (!m_freed) {
		m_factory.internalUnlisten(m_symbolTable);
		m_freed = true;
	}
}
