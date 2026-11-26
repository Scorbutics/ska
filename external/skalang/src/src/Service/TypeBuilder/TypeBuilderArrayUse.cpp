#include "Config/LoggerConfigLang.h"
#include "TypeBuilderArrayUse.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>::build(const ScriptAST& script, OperateOn node) {
    // A character of a string is a string
    if (node.GetArray().type() == ExpressionType::STRING) {
        return Type::MakeBuiltIn<ExpressionType::STRING>();
    }
    // A value of a map, an element of an array
    return node.GetArraySubType();
}
