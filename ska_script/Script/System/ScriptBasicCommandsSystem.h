#pragma once
#include "ScriptAutoSystem.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
    class MemoryScript;
	class TileWorld;

	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		
	public :
		ScriptBasicCommandsSystem(ska::TileWorld& w, EntityManager& entityManager, const EntityLocator& locator, GameEventDispatcher& ged, MemoryScript& saveGame);
		~ScriptBasicCommandsSystem() override = default;

	protected:
		ScriptBasicCommandsSystem(ska::TileWorld& w, EntityManager& entityManager, GameEventDispatcher& ged, const ScriptCommandHelper& sch, MemoryScript& saveGame);

		struct BasicScriptCommandHelper : public ScriptCommandHelper {
			friend class ScriptBasicCommandsSystem;
		    BasicScriptCommandHelper() = default;		
			BasicScriptCommandHelper(TileWorld& w, EntityManager& entityManager, const EntityLocator& locator) :
				ScriptCommandHelper(entityManager),
				m_world(w),
				m_locator(locator) {
			}

			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual ~BasicScriptCommandHelper() = default;

			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& c) const override;

		private:
			TileWorld& m_world;
		protected:
			const EntityLocator& m_locator;
		};
	};

}
