#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include "Core/ECS/ECSDefines.h"
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/ECSEvent.h"
#include "Core/Data/Events/GameEventDispatcher.h"

namespace ska {
	namespace scripting {

		// Entities given to the scripts : an entity handle is an "int" made of the entity id and of its generation.
		// The engine reuses the id of a removed entity : a script keeping the handle of a removed entity (ex : across a wait)
		// gets an error when using it, instead of acting on another entity.
		// A handle is never negative : -1 means "no entity".
		class EntityHandles :
			public SubObserver<ECSEvent> {
		public:
			explicit EntityHandles(GameEventDispatcher& ged);
			~EntityHandles() override = default;

			// Handle of an existing entity
			long handle(EntityId entity) const;
			// Handle, or -1 when there is no entity
			long handle(const std::optional<EntityId>& entity) const;
			// Entity of a handle, when it was not removed since the handle was given
			std::optional<EntityId> find(long handle) const;
			// Entity of a handle : throws when the handle is invalid or when its entity was removed
			EntityId entity(long handle) const;

		private:
			static constexpr long IdRange = 1L << 16;
			// Generations stay positive in a 32 bits int (long on Windows)
			static constexpr std::uint16_t GenerationRange = 1u << 15;
			static_assert(SKA_ECS_MAX_ENTITIES <= IdRange, "entity ids must fit in an entity handle");

			bool onECSEvent(ECSEvent& ecsEvent);

			std::array<std::uint16_t, SKA_ECS_MAX_ENTITIES> m_generations {};
		};
	}
}
