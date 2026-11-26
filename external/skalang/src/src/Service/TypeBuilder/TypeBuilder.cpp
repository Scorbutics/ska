#include "Config/LoggerConfigLang.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "NodeValue/AST.h"
#include "Service/ASTFactory.h"

#include "NodeValue/ScriptAST.h"
#include "Service/ScopedSymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, TypeBuilder)

ska::TypeBuilder::TypeBuilder(StatementParser& parser, const TypeCrosser& typeCrosser) :
  subobserver_priority_queue<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<FunctionTokenEvent>(std::bind(&TypeBuilder::matchFunction, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<VarTokenEvent>(std::bind(&TypeBuilder::matchVariable, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ReturnTokenEvent>(std::bind(&TypeBuilder::matchReturn, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ArrayTokenEvent>(std::bind(&TypeBuilder::matchArray, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<FilterTokenEvent>(std::bind(&TypeBuilder::matchFilter, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ScriptLinkTokenEvent>(std::bind(&TypeBuilder::matchScriptLink, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ConverterTokenEvent>(std::bind(&TypeBuilder::matchConverter, this, std::placeholders::_1), parser, 6) {
	m_typeBuilder = BuildTypeBuildersContainer(typeCrosser);
}

bool ska::TypeBuilder::matchFilter(FilterTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for variable \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for variable \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type {}) << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}


bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for variable \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for variable \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type{}) << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}
void ska::TypeBuilder::buildType(ASTNode& node, ScriptAST& script) {
	if (node.type().has_value()) {
		return;
	}

	for (auto& child : node) {
		buildType(*child, script);
	}

	auto& typeBuilder = m_typeBuilder[static_cast<std::size_t>(node.op())];
	assert(typeBuilder != nullptr && "Cannot calculate the node type (it might be an empty node)");
	auto typeHierarchy = typeBuilder->build(script, node);

	// We have to const_cast here, because the whole TypeBuilder system doesn't modify any value, including symbol table, while computing operations.
	// So we cannot have a Symbol* in return (TypeHierarchy) type, but only a const Symbol*, even if the original stored SymbolTable is still mutable.
	// TODO : find a way to avoid the const_cast, maybe by maintaining an unordered_map<const Symbol*, Symbol*>?
	auto* computedSymbolTable = const_cast<ScopedSymbolTable*>(typeHierarchy.link());
	auto* currentSymbolTable = node.symbolTable();
	if (computedSymbolTable != nullptr && computedSymbolTable->symbol() != nullptr) {
		if (currentSymbolTable == nullptr) {
			SLOG(LogLevel::Warn) << "Symbol link \"" << computedSymbolTable->symbol()->type() << "\" for node with OP \"" << node.op() << "\"";
			node.updateType(Type::Override(std::move(typeHierarchy.type), computedSymbolTable));
			node.linkSymbol(*computedSymbolTable);
			return;
		} else if (computedSymbolTable != currentSymbolTable) {
			SLOG(LogLevel::Warn) << "%14cSymbol link \"" << computedSymbolTable->name() << "\" now implements \"" << currentSymbolTable->name() << "\"";
			node.updateType(typeHierarchy.type);
			computedSymbolTable->implement(*currentSymbolTable);
			return;
		}
	}

	node.updateType(typeHierarchy.type);
}

bool ska::TypeBuilder::matchReturn(ReturnTokenEvent& event) {
	if (event.type() != ReturnTokenEventType::START) {
		auto& node = event.rootNode();
		buildType(node, event.script());
	}
	return true;
}

bool ska::TypeBuilder::matchArray(ArrayTokenEvent & event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type array (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for array = \"" << event.rootNode().type().value_or(Type{}) << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchScriptLink(ScriptLinkTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for script link \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for script link \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type{})  << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for expression \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for expression \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type{})  << "\" (Operator " << event.rootNode().op() << ")";
  return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) {
	if (event.type() != FunctionTokenEventType::DECLARATION_NAME) {
		auto& node = event.rootNode();
		SLOG(LogLevel::Debug) << "Building type for function parameter declaration / call \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
		buildType(node, event.script());
		SLOG(LogLevel::Info) << "Type built for function parameter declaration / call \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type{})  << "\" (Operator " << event.rootNode().op() << ")";
	}
  return true;
}

bool ska::TypeBuilder::matchConverter(ConverterTokenEvent& event) {
	SLOG(LogLevel::Debug) << "Building type for converter / call \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(event.rootNode(), event.script());
	SLOG(LogLevel::Info) << "Type built for converter call \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value_or(Type {}) << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}
