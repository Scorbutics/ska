#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandBlockAuthorize :
		public AbstractFunctionCommand {
	public:
		explicit CommandBlockAuthorize(EntityManager& e);
		void operator=(const CommandBlockAuthorize&) = delete;
		virtual ~CommandBlockAuthorize() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
