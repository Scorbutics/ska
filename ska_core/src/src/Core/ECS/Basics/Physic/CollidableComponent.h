#pragma once
#include <unordered_set>

#include "Core/ECS/Component.h"
namespace ska {
	class CollidableComponent {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		
		std::unordered_set<int> authorizedBlockIds;
		bool ghost;
	};
}

SKA_DECLARE_COMPONENT(ska::CollidableComponent);