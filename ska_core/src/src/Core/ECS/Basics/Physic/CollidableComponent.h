#pragma once
#include <unordered_set>

#include "Core/ECS/ComponentTag.h"
namespace ska {
	class CollidableComponent {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		
		std::unordered_set<int> authorizedBlockIds;
		bool ghost;
	};

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<CollidableComponent>;
#endif
}
