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
		// The name of an entity ("" when it has none)
		std::string getName(EntityId entity) const {
			for (const auto& [name, id] : m_entitiesNameMap) {
				if (id == entity) {
					return name;
				}
			}
			return "";
		}

	private:
		bool onECSEvent(ECSEvent& ecsEvent);
		void forget(EntityId entity);

		std::array<std::string, SKA_ECS_MAX_ENTITIES> m_entitiesReverseNameMap;
		std::unordered_map<std::string, EntityId> m_entitiesNameMap;
	};
}