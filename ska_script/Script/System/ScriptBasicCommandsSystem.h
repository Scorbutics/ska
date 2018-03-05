#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
    class MemoryScript;
	class TileWorld;
	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(EntityManager& entityManager, TileWorld& w, MemoryScript& saveGame);
		virtual ~ScriptBasicCommandsSystem();

	protected:
		ScriptBasicCommandsSystem(EntityManager& entityManager, TileWorld& w, const ScriptCommandHelper& sch, MemoryScript& saveGame);
		struct BasicScriptCommandHelper : public ScriptCommandHelper {
		    BasicScriptCommandHelper() = default;
		    virtual ~BasicScriptCommandHelper() = default;
			BasicScriptCommandHelper(TileWorld& w, EntityManager& entityManager) : ScriptCommandHelper(w, entityManager) {}
			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual void setupCommands(TileWorld& w, std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}
