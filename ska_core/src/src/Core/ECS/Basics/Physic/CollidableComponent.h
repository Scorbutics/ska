#pragma once
#include <unordered_set>

#include "Core/ECS/Component.h"
namespace ska {
	class CollidableComponent : public Component<CollidableComponent> {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		
		std::unordered_set<int> authorizedBlockIds;
		bool ghost;
	};
}
