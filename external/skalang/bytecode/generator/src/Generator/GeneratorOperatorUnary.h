#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeUnary.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(ska::Operator::UNARY);
		SKALANG_GENERATOR_OPERATOR_DEFINE(ska::Operator::LITERAL);
	}
}
