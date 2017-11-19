#pragma once
#include <unordered_set>
#include "../../SerializableComponent.h"

namespace ska {
	class CollidableComponent : public SerializableComponent {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		
		std::unordered_set<int> authorizedBlockIds;
		bool ghost;
	};
}
