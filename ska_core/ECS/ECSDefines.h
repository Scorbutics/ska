#pragma once

#include <vector>

namespace ska {
	constexpr static auto SKA_ECS_MAX_ENTITIES = 1000;
	constexpr static auto SKA_ECS_MAX_COMPONENTS = 50;

	using ComponentId = std::size_t;

	using EntityId = std::size_t;

	using EntityIdContainer = std::vector<EntityId>;
}
