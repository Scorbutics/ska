#include <sstream>
#include <unordered_map>
#include "Config/LoggerConfigLang.h"
#include "ExpressionTypeCrossBasic.h"
#include "Service/LanguageHints.h"

namespace ska {
	namespace typedetail {
		struct CrossLogger;
	}
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::typedetail::CrossLogger)

namespace ska {
	namespace typedetail {
		constexpr auto TypeMapSize = static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length);
		using TypeMap = std::unordered_map<std::string, int(*)[TypeMapSize][TypeMapSize]>;
		
		// Strong typing : the operators only take operands of the same type (no implicit conversion : a conversion is
		// explicit, with the converter ":"). Arrays : concatenation and pushing / removing an element.
		const TypeMap& GetMap(const std::string& op) {
			static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 4, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 5, 6, 0, 0 },
				{ 0, 6, 0, 6, 6, 6, 6, 6, 0 },
				{ 0, 0, 0, 0, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 4, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 6, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 5, 0, 0, 0 },
				{ 0, 0, 0, 0, 4, 0, 0, 0, 0 },
				{ 0, 0, 0, 5, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 4, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorModulo[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};
			static int typeMapOperatorEqual[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 2, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 4, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 5, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 8 }
			};

			static int typeMapOperatorEquality[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 7, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 7, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 7, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 7, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 7, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 7, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorLogical[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 7, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 7, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 7, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 7, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 7, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorLogicalBool[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorConvert[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 1, 0, 1, 1, 1, 1, 1, 0 },
				{ 0, 0, 2, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 6, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 8 }
			};

			static auto typeMap = std::unordered_map<std::string, int(*)[TypeMapSize][TypeMapSize]> {};
			typeMap.emplace("+", &typeMapOperatorPlus);
			typeMap.emplace("-", &typeMapOperatorMinus);
			typeMap.emplace("*", &typeMapOperatorMul);
			typeMap.emplace("/", &typeMapOperatorDiv);
			typeMap.emplace("%", &typeMapOperatorModulo);
			typeMap.emplace("=", &typeMapOperatorEqual);
			typeMap.emplace(":", &typeMapOperatorConvert);
			typeMap.emplace("==", &typeMapOperatorEquality);
			typeMap.emplace("!=", &typeMapOperatorEquality);
			typeMap.emplace("&&", &typeMapOperatorLogicalBool);
			typeMap.emplace("||", &typeMapOperatorLogicalBool);
			typeMap.emplace("<", &typeMapOperatorLogical);
			typeMap.emplace("<=", &typeMapOperatorLogical);
			typeMap.emplace(">", &typeMapOperatorLogical);
			typeMap.emplace(">=", &typeMapOperatorLogical);
			return typeMap;
		}
	}
}

ska::ExpressionType ska::typedetail::ExpressionTypeCross(const std::string& op, ExpressionType type1, ExpressionType type2) {
    static const auto& typeMap = GetMap(op);

	if (typeMap.find(op) == typeMap.end()) {
        auto ss = std::stringstream{};
        ss << "unknown operator " << op << hints::UnknownOperator(op);
        throw std::runtime_error(ss.str());
	}

	const auto typeIdResult = (*typeMap.at(op))[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2)];
	if (typeIdResult == 0) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types \"" << ExpressionTypeSTR[static_cast<std::size_t>(type1)] << "\" and \"" << ExpressionTypeSTR[static_cast<std::size_t>(type2)] << "\"" << (type1 != type2 ? hints::DifferentTypes() : "");
		throw std::runtime_error(ss.str());
	}
	
	SLOG_STATIC(LogLevel::Info, CrossLogger) << op << " has cross-type " << ExpressionTypeSTR[static_cast<std::size_t>(typeIdResult)];

	return static_cast<ExpressionType>(typeIdResult);	
}
