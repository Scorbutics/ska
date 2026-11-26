#pragma once

#include "NodeValue/TypeHierarchy.h"
#include "TypeBuildUnit.h"
#include "Operation/OperationType.h"

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(OperatorType)\
    template<>\
    struct TypeBuilderOperator<OperatorType> : \
		public TypeBuildUnit {\
	private:\
		using OperateOn = OperationType<OperatorType>;\
	public:\
        TypeHierarchy build(const ScriptAST& script, ASTNode& node) override final {\
			return build(script, OperateOn{node});\
		}\
		TypeHierarchy build(const ScriptAST& script, OperateOn node);\
    };

namespace ska {
    template <Operator O>
    struct TypeBuilderOperator;
}
