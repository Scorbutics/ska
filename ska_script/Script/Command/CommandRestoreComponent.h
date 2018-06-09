#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandRestoreComponent :
		public AbstractFunctionCommand {
	public:
		explicit CommandRestoreComponent(EntityManager& e);
		void operator=(const CommandRestoreComponent&) = delete;
		virtual ~CommandRestoreComponent() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
