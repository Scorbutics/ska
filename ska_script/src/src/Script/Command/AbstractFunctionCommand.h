#pragma once
#include "AbstractCommand.h"

namespace ska {
    class MemoryScript;

	class AbstractFunctionCommand :
		public AbstractCommand {
	public:
		explicit AbstractFunctionCommand(EntityManager& entityManager);
		void operator=(const AbstractFunctionCommand&) = delete;
		virtual ~AbstractFunctionCommand();

	protected:
		virtual int argumentsNumber() = 0;
		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) = 0;

		std::string process(ScriptComponent& script, MemoryScript& memory, std::stringstream& streamCmd, const std::vector<std::string>& args) override;
		virtual char getSeparator() override;
	};
}
