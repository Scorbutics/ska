#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/OperationTypeArrayUse.h"

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(OperatorType) \
    template <>\
    struct TypeBuilderOperator<OperatorType> : public TypeBuildUnit {\
        TypeHierarchy build(const ScriptAST& script, ASTNode& node) override {\
			return Type{ };\
        }\
    };

namespace ska {
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::FOR_LOOP);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::BLOCK);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::IF);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::IF_ELSE);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::FILTER);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE_DEFAULT(Operator::FILTER_DECLARATION);
}
