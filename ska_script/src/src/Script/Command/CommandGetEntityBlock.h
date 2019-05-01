#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class TileWorld;

	class CommandGetEntityBlock :
		public AbstractFunctionCommand {
	public:
		explicit CommandGetEntityBlock(TileWorld& world, EntityManager& e);
		void operator=(const CommandGetEntityBlock&) = delete;
		virtual ~CommandGetEntityBlock() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	private:
		TileWorld& m_world;
	};
}
