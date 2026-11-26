#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/OperationTypeUserDefinedObject.h"
#include "Operation/OperationTypeReturn.h"

namespace ska {
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::USER_DEFINED_OBJECT);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::RETURN);
}

