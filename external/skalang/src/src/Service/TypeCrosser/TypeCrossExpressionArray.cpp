#include <sstream>
#include "TypeCrossExpressionArray.h"

ska::Type ska::TypeCrossExpression<ska::ExpressionType::ARRAY>::cross(const std::string& op, const Type& type1, const Type& type2) const {
	//Handles array deleting cells
	if(op == "-" && type1 == ExpressionType::ARRAY) {
		//Either delete a range of values or an index
		if (type2 == ExpressionType::INT) {
			return type1;
		}
		
		if(type2 == ExpressionType::ARRAY) {
			if (type2[0] != ExpressionType::INT) {
				auto ss = std::stringstream{};
				ss << "Unable to use operator \"" << op << "\" on types \"" << type1 << "\" and \"" << type2 << "\"";
				throw std::runtime_error(ss.str());
			}
			return type1;
		}
	}
	
	// Arrays of functions are compatible when their functions are (same rule as for a function : structural equality,
	// the names of the functions do not matter). Ex : an array of callbacks given to a parameter typed as "Default[]".
	const auto functionArraysCompatible = type1 == ExpressionType::ARRAY && type2 == ExpressionType::ARRAY &&
		type1.size() > 0 && type2.size() > 0 &&
		type1[0] == ExpressionType::FUNCTION && type2[0] == ExpressionType::FUNCTION &&
		type1[0].structuralEquality(type2[0]);
	if (functionArraysCompatible) {
		return type1;
	}

	const auto checkArraySubTypeCoherence = (type1 == type2) || (type2.size() > 0 ? type2[0] == type1 : (type1.size() > 0 && type1[0] == type2));
	if (!checkArraySubTypeCoherence) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types \"" << type1 << "\" and \"" << type2 << "\"";
		throw std::runtime_error(ss.str());
	}

	return type2.size() > 0 ? type2 : type1;
}
