#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeIfElse.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(ska::Operator::IF);
		SKALANG_GENERATOR_OPERATOR_DEFINE(ska::Operator::IF_ELSE);
	}
}
