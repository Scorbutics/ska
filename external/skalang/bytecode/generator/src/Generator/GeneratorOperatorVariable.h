#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeVariableAffectation.h"
#include "Operation/OperationTypeVariableDeclaration.h"
#include "Operation/OperationTypeParameterDeclaration.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::AFFECTATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::DECLARATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::PARAMETER_DECLARATION);
	}
}
