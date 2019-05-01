#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandScript :
		public AbstractFunctionCommand {
	public:
		explicit CommandScript(EntityManager& e);
		void operator=(const CommandScript&) = delete;
		virtual ~CommandScript() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};

}
