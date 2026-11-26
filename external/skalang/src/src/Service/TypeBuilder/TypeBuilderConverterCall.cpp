#include "Config/LoggerConfigLang.h"
#include "TypeBuilderConverterCall.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::CONVERTER_CALL>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::CONVERTER_CALL>::build(const ScriptAST& script, OperateOn node) {
	return node.GetCalledConverterType();
}
