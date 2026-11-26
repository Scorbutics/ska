#include "Config/LoggerConfigLang.h"
#include "TypeBuilderMapDeclaration.h"
#include "NodeValue/AST.h"

// [:]: T : a map of T values
ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::MAP_DECLARATION>::build(const ScriptAST&, OperateOn node) {
	auto type = Type::MakeBuiltIn<ExpressionType::MAP>();
	type.add(node.GetMapSubType());
	return type;
}
