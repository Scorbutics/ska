#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class TileWorld;

	class CommandHasBlockProperty :
		public AbstractFunctionCommand {
	public:
		explicit CommandHasBlockProperty(TileWorld& w, EntityManager& e);
		void operator=(const CommandHasBlockProperty&) = delete;
		virtual ~CommandHasBlockProperty() = default;

		std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		int argumentsNumber() override;
	
	private:
		TileWorld& m_world;
	};
}
