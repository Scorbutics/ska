#include "ScriptBasicCommandsSystem.h"

#include "../Command/CommandEnd.h"
#include "../Command/CommandCalculate.h"
#include "../Command/CommandAssignSwitch.h"
#include "../Command/CommandEndScript.h"
#include "../Command/CommandAssign.h"
#include "../Command/CommandRandom.h"
#include "../Command/CommandWait.h"
#include "../Command/CommandLog.h"
#include "../Command/CommandScript.h"
#include "../Command/CommandIf.h"
#include "../Command/CommandElse.h"
#include "../Command/CommandElseEnd.h"
#include "../Command/CommandRemoveComponent.h"
#include "../Command/CommandRestoreComponent.h"
#include "../Command/CommandBlockAuthorize.h"

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(EntityManager& entityManager, MemoryScript& saveGame) :
	ScriptAutoSystem(entityManager, BasicScriptCommandHelper(entityManager), saveGame) {

}

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(EntityManager& entityManager, const ScriptCommandHelper& sch, MemoryScript& saveGame) :
	ScriptAutoSystem(entityManager, sch, saveGame) {

}

void ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper::setupCommands(std::unordered_map<std::string, CommandPtr>& c) const {
	c[CommandEnd::getCmdName()] = std::unique_ptr<Command>(std::make_unique<CommandEnd>(m_entityManager));
	c[ControlStatement::getCommandIf()] = std::unique_ptr<Command>(std::make_unique<CommandIf>(m_entityManager));
	c[ControlStatement::getCommandElse()] = std::unique_ptr<Command>(std::make_unique<CommandElse>(m_entityManager));
	c[ControlStatement::getCommandEndIf()] = std::unique_ptr<Command>(std::make_unique<CommandElseEnd>(m_entityManager));
	c["end_script"] = std::unique_ptr<Command>(std::make_unique<CommandEndScript>(m_entityManager));
	c["calculate"] = std::unique_ptr<Command>(std::make_unique<CommandCalculate>(m_entityManager));
	c["assign"] = std::unique_ptr<Command>(std::make_unique<CommandAssign>(m_entityManager));
	c["assign_switch"] = std::unique_ptr<Command>(std::make_unique<CommandAssignSwitch>(m_entityManager));
	c["random"] = std::unique_ptr<Command>(std::make_unique<CommandRandom>(m_entityManager));
	c["log"] = std::unique_ptr<Command>(std::make_unique<CommandLog>(m_entityManager));
	c["script"] = std::unique_ptr<Command>(std::make_unique<CommandScript>(m_entityManager));
	c["wait"] = std::unique_ptr<Command>(std::make_unique<CommandWait>(m_entityManager));
	c["remove_component"] = std::unique_ptr<Command>(std::make_unique<CommandRemoveComponent>(m_entityManager));
	c["restore_component"] = std::unique_ptr<Command>(std::make_unique<CommandRestoreComponent>(m_entityManager));
	c["block_authorize"] = std::unique_ptr<Command>(std::make_unique<CommandBlockAuthorize>(m_entityManager));
}
