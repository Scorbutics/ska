#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class EntityLocator;

	class CommandGetEntityId :
		public AbstractFunctionCommand {
	public:
		CommandGetEntityId(EntityManager& e, const EntityLocator& locator);
		void operator=(const CommandGetEntityId&) = delete;
		virtual ~CommandGetEntityId() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;

	private:
		const EntityLocator& m_locator;
	};
}
