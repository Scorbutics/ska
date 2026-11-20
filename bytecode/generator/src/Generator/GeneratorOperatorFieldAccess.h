#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeFieldAccess.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(ska::Operator::FIELD_ACCESS);

		// Index of the field accessed by a custom object field access node (throws if the field does not exist)
		Operand FieldAccessIndex(OperationType<Operator::FIELD_ACCESS> node, const GenerationContext& context);
	}
}
