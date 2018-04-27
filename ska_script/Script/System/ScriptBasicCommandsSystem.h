#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
    class MemoryScript;
	class TileWorld;
	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(EntityManager& entityManager, MemoryScript& saveGame);
		~ScriptBasicCommandsSystem() override = default;

	protected:
		ScriptBasicCommandsSystem(EntityManager& entityManager, const ScriptCommandHelper& sch, MemoryScript& saveGame);
		struct BasicScriptCommandHelper : public ScriptCommandHelper {
		    BasicScriptCommandHelper() = default;
		    virtual ~BasicScriptCommandHelper() = default;
			BasicScriptCommandHelper(EntityManager& entityManager) : ScriptCommandHelper(entityManager) {}
			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}
