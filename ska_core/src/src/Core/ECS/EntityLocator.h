#pragma once
#include <array>
#include <unordered_map>
#include "ECSDefines.h"
#include "Base/Patterns/SubObserver.h"
#include "../Data/Events/ECSEvent.h"
#include "../Data/Events/GameEventDispatcher.h"

namespace ska {
	class EntityLocator :
		public SubObserver<ECSEvent> {
	public:
		EntityLocator(GameEventDispatcher& ged);
		~EntityLocator() = default;

		const EntityId* getEntityId(const std::string& name) const;

	private:
		bool onECSEvent(ECSEvent& ecsEvent);

		std::array<std::string, SKA_ECS_MAX_ENTITIES> m_entitiesReverseNameMap;
		std::unordered_map<std::string, EntityId> m_entitiesNameMap;
	};
}