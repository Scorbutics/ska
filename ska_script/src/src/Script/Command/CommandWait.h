#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandWait :
		public AbstractFunctionCommand {
	public:
		explicit CommandWait(EntityManager& e);
		void operator=(const CommandWait&) = delete;
		virtual ~CommandWait() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};

}
