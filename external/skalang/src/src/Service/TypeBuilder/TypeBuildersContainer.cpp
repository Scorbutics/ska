#include <cassert>
#include "TypeBuildUnit.h"
#include "TypeBuilderOperator.h"
#include "TypeBuildersContainer.h"

#include "Service/TypeBuilder/TypeBuilderConverterCall.h"
#include "Service/TypeBuilder/TypeBuilderFunctionCall.h"
#include "Service/TypeBuilder/TypeBuilderFunctionPrototypeDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderFunctionDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderParameterDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderArrayDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderArrayUse.h"
#include "Service/TypeBuilder/TypeBuilderVariableAffectation.h"
#include "Service/TypeBuilder/TypeBuilderVariableDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderImport.h"
#include "Service/TypeBuilder/TypeBuilderExport.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/TypeBuilder/TypeBuilderScriptLink.h"
#include "Service/TypeBuilder/TypeBuilderScriptObject.h"
#include "Service/TypeBuilder/TypeBuilderLiteral.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "Service/TypeBuilder/TypeBuilderType.h"
#include "Service/TypeBuilder/TypeBuilderMapDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderLanguageValues.h"
#include "Service/TypeBuilder/TypeBuilderUserDefinedObject.h"
#include "Service/TypeBuilder/TypeBuilderFilterParameterDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderFactoryPrototypeDeclaration.h"

#include "Service/TypeBuilder/TypeBuilderDefaults.h"

namespace ska {
	template<Operator op, class ... Args >
	inline void TypeBuilderMakeBuilder(TypeBuildersContainer& result, Args&& ... args) {
		result[static_cast<std::size_t>(op)] = std::make_unique<TypeBuilderOperator<op>>(std::forward<Args>(args)...);
	}
}

ska::TypeBuildersContainer ska::BuildTypeBuildersContainer(const TypeCrosser& typeCrosser) {
	auto result = TypeBuildersContainer {};
	TypeBuilderMakeBuilder<Operator::BINARY>(result, typeCrosser);
	TypeBuilderMakeBuilder<Operator::UNARY>(result);
	TypeBuilderMakeBuilder<Operator::LITERAL>(result);
	TypeBuilderMakeBuilder<Operator::DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::AFFECTATION>(result);
	TypeBuilderMakeBuilder<Operator::PARAMETER_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::ARRAY_TYPE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::ARRAY_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::ARRAY_USE>(result);
	TypeBuilderMakeBuilder<Operator::MAP_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::MAP_LITERAL>(result);
	TypeBuilderMakeBuilder<Operator::ENUM_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::MATCH>(result);
	TypeBuilderMakeBuilder<Operator::MATCH_BRANCH>(result);
	TypeBuilderMakeBuilder<Operator::USER_DEFINED_OBJECT>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_CALL>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_PROTOTYPE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FACTORY_PROTOTYPE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FIELD_ACCESS>(result);
	TypeBuilderMakeBuilder<Operator::FOR_LOOP>(result);
	TypeBuilderMakeBuilder<Operator::IMPORT>(result);
	TypeBuilderMakeBuilder<Operator::EXPORT>(result);
	TypeBuilderMakeBuilder<Operator::BLOCK>(result);
	TypeBuilderMakeBuilder<Operator::IF>(result);
	TypeBuilderMakeBuilder<Operator::IF_ELSE>(result);
	TypeBuilderMakeBuilder<Operator::RETURN>(result);
	TypeBuilderMakeBuilder<Operator::SCRIPT_LINK>(result);
	TypeBuilderMakeBuilder<Operator::SCRIPT_OBJECT>(result);
	TypeBuilderMakeBuilder<Operator::TYPE>(result);
	TypeBuilderMakeBuilder<Operator::FILTER>(result);
	TypeBuilderMakeBuilder<Operator::CONVERTER_CALL>(result);
	TypeBuilderMakeBuilder<Operator::FILTER_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FILTER_PARAMETER_DECLARATION>(result);
#ifndef NDEBUG
	for (std::size_t i = 0; i < MaxOperatorSize; i++) {
		assert(result[i] != nullptr);
	}
#endif
	return result;
}