#pragma once
#include <unordered_set>

namespace ska {
	class CollidableComponent {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		
		std::unordered_set<int> authorizedBlockIds;
		bool ghost;
	};
}
