#pragma once

#include "NodeValue/Operator.h"

namespace ska {
	struct OperatorTraits {

		static constexpr bool isNamed(Operator o) {
			switch (o) {
				case Operator::FACTORY_PROTOTYPE_DECLARATION:
				case Operator::FUNCTION_PROTOTYPE_DECLARATION:
				case Operator::FUNCTION_DECLARATION:
				case Operator::DECLARATION:
				case Operator::PARAMETER_DECLARATION:
				case Operator::FILTER_PARAMETER_DECLARATION:
					return true;
				default:
					return false;
			}
		}

		static constexpr bool isLvalueCompatible(Operator o) {
			switch (o) {
			case Operator::UNARY:
			case Operator::FIELD_ACCESS:
			case Operator::FUNCTION_DECLARATION:
			case Operator::AFFECTATION:
			case Operator::DECLARATION:
			case Operator::PARAMETER_DECLARATION:
			case Operator::ARRAY_USE:
			case Operator::ARRAY_DECLARATION:
				return true;
			default:
				return false;
			}
		}
	};
}