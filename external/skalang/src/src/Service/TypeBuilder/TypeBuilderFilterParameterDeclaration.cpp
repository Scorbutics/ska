#include "TypeBuilderFilterParameterDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FILTER_PARAMETER_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto collectionType = node.GetCollectionType();

	if (collectionType == ExpressionType::VOID) {
		return Type::MakeBuiltIn<ExpressionType::INT>();
	}

	if (collectionType == ExpressionType::ARRAY) {
		// The iterator node is linked to its own variable symbol : the type of an object element keeps its class
		// (the iterator variable then implements it, as any variable declared from an object value)
		return TypeHierarchy{ collectionType[0] };
	}

	auto ss = std::stringstream{};
	ss << "invalid collection for the current filter : expected an array but got \"" << collectionType << "\" instead";
	throw std::runtime_error(ss.str());
}
