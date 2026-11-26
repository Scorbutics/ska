#include "Config/LoggerConfigLang.h"
#include "TypeCrossExpression.h"

namespace ska {
	namespace typedetail {
		template <ExpressionType t>
		inline void TypeCrosserMakeCross(TypeCrosser::TypeCrossMap& result) {
			result[static_cast<std::size_t>(t)] = std::make_unique<TypeCrossExpression<t>>();
		}
	}
}


SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeCrosser)

ska::TypeCrosser::TypeCrossMap ska::TypeCrosser::typeCrossMap = ska::TypeCrosser::BuildTypeCrossMap();

ska::TypeCrosser::TypeCrossMap ska::TypeCrosser::BuildTypeCrossMap() {
	auto result = TypeCrossMap{};
	typedetail::TypeCrosserMakeCross<ExpressionType::ARRAY>(result);
	typedetail::TypeCrosserMakeCross<ExpressionType::MAP>(result);
	typedetail::TypeCrosserMakeCross<ExpressionType::OBJECT>(result);
	typedetail::TypeCrosserMakeCross<ExpressionType::FUNCTION>(result);
	return result;
}

ska::Type ska::TypeCrosser::cross(const std::string& op, const Type& type1, const Type& type2) const {
	SLOG(ska::LogLevel::Info) << "Crossing types between \"" << type1 << " and \"" << type2 << "\" with operator \"" << op << "\"";
	const auto basicComputedExpressionType = typedetail::ExpressionTypeCross(op, type1.type(), type2.type());
	SLOG(ska::LogLevel::Info) << "Crossing types basic result = \"" << Type{basicComputedExpressionType} << "\"";
	const auto& complexTypeCrosser = typeCrossMap[static_cast<std::size_t>(basicComputedExpressionType)];
	if(complexTypeCrosser != nullptr) {
		auto result = complexTypeCrosser->cross(op, type1, type2);
		SLOG(ska::LogLevel::Info) << "Crossing types full result = \"" << result << "\"";
		return result;
	}
	return Type{ basicComputedExpressionType };
}
