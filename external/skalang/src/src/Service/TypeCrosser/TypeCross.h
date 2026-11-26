#pragma once
#include <memory>
#include "NodeValue/Type.h"

namespace ska {
	class TypeCross {
		public:
		TypeCross() = default;
		virtual ~TypeCross() = default;
		virtual Type cross(const std::string& op, const Type& type1, const Type& type2) const = 0;
	};
	
	using TypeCrossPtr = std::unique_ptr<TypeCross>;
}

