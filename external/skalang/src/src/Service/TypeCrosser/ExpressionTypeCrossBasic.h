#pragma once
#include <string>
#include "NodeValue/ExpressionType.h"

namespace ska {
	namespace typedetail {
		ExpressionType ExpressionTypeCross(const std::string& op, ExpressionType type1, ExpressionType type2);		
	};
}
