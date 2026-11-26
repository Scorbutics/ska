#pragma once
#include <array>
#include "NodeValue/Operator.h"
#include "TypeBuildUnitPtr.h"

namespace ska {
	class TypeCrosser;
	static constexpr auto MaxOperatorSize = static_cast<std::size_t>(Operator::UNUSED_Last_Length);
	using TypeBuildersContainer = std::array<TypeBuildUnitPtr, MaxOperatorSize>;
	TypeBuildersContainer BuildTypeBuildersContainer(const TypeCrosser& typeCrosser);
}
