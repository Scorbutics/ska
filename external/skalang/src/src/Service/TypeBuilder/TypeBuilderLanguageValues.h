#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/OperationTypeMapLiteral.h"
#include "Operation/OperationTypeEnumDeclaration.h"
#include "Operation/OperationTypeMatch.h"
#include "Operation/OperationTypeMatchBranch.h"

namespace ska {
	SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::MAP_LITERAL);
	SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::ENUM_DECLARATION);
	SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::MATCH);
	SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::MATCH_BRANCH);
}
