#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
    class MemoryScript;
	class TileWorld;

	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(ska::TileWorld& w, EntityManager& entityManager, MemoryScript& saveGame);
		~ScriptBasicCommandsSystem() override = default;

	protected:
		ScriptBasicCommandsSystem(ska::TileWorld& w, EntityManager& entityManager, const ScriptCommandHelper& sch, MemoryScript& saveGame);

		struct BasicScriptCommandHelper : public ScriptCommandHelper {
		    BasicScriptCommandHelper() = default;		
			BasicScriptCommandHelper(TileWorld& w, EntityManager& entityManager) : 
				ScriptCommandHelper(entityManager),
				m_world(w) {
			}

			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual ~BasicScriptCommandHelper() = default;

			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& c) const override;

		private:
			TileWorld& m_world;
		};
		
	};

}
