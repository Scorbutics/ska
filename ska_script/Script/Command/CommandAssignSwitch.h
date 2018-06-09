#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {

	class CommandAssignSwitch :
		public AbstractFunctionCommand {
	public:
		explicit CommandAssignSwitch(EntityManager& entityManager);
		void operator=(const CommandAssignSwitch&) = delete;
		virtual ~CommandAssignSwitch() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
