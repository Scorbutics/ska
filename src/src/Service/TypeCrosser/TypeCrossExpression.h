#pragma once
#include <array>
#include "TypeCross.h"
#include "ExpressionTypeCrossBasic.h"

#define SKALANG_INTERPRETER_TYPE_CROSS_DEFINE(ExpressionType_)\
    template<>\
    class TypeCrossExpression<ExpressionType_> : \
		public TypeCross {\
	public:\
		using TypeCross::TypeCross;\
		~TypeCrossExpression() override = default;\
        Type cross(const std::string& op, const Type& type1, const Type& type2) const override final;\
    };

namespace ska {
	class TypeCrosser : 
		public TypeCross {
	public:
		static constexpr auto TypeCrossMapSize = static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length);
		using TypeCrossMap = std::array<TypeCrossPtr, TypeCrossMapSize>;
		TypeCrosser() = default;
		TypeCrosser(const TypeCrosser&) = delete;
		TypeCrosser(TypeCrosser&&) = delete;
		TypeCrosser& operator=(const TypeCrosser&) = delete;
		TypeCrosser& operator=(TypeCrosser&&) = delete;
		~TypeCrosser() override = default;

		Type cross(const std::string& op, const Type& type1, const Type& type2) const override final;
	private:
		
		static TypeCrossMap BuildTypeCrossMap();
		
		static TypeCrossMap typeCrossMap;
	};

	template <ExpressionType O>
	class TypeCrossExpression : 
		public TypeCross {
	public:
		Type cross(const std::string& op, const Type& type1, const Type& type2) const override final {
			return Type{};
		}
	};
}

#include "TypeCrossExpressionArray.h"
#include "TypeCrossExpressionMap.h"
#include "TypeCrossExpressionObject.h"
#include "TypeCrossExpressionFunction.h"
