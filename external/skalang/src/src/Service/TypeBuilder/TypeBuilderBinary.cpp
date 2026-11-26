#include "TypeBuilderBinary.h"

#include "NodeValue/AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::BINARY>::build(const ScriptAST& script, OperateOn node) {
    assert(!node.GetOperator().empty());
    const auto& type1 = node.GetFirstType();
    const auto& type2 = node.GetSecondType();
	return type1.crossTypes(m_typeCrosser, node.GetOperator(), type2);
}

ska::TypeBuilderOperator<ska::Operator::BINARY>::TypeBuilderOperator(const TypeCrosser& typeCrosser) :
	m_typeCrosser(typeCrosser) {
}