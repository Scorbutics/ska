#include <sstream>
#include "TypeCrossExpressionMap.h"

// A map is only assigned a map with the same type of values
ska::Type ska::TypeCrossExpression<ska::ExpressionType::MAP>::cross(const std::string& op, const Type& type1, const Type& type2) const {
	if (!type1.structuralEquality(type2)) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types \"" << type1 << "\" and \"" << type2 << "\"";
		throw std::runtime_error(ss.str());
	}
	return type1;
}
