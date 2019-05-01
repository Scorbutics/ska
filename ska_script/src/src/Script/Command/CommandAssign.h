#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandAssign :
		public AbstractFunctionCommand {
	public:
		explicit CommandAssign(EntityManager& entityManager);
		void operator=(const CommandAssign&) = delete;
		virtual ~CommandAssign() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
