#pragma once
#include "ControlStatement.h"

namespace ska {
	class CommandElse :
		public ControlStatement {
	public:
		explicit CommandElse(EntityManager& e);
		void operator=(const CommandElse&) = delete;
		virtual ~CommandElse() = default;

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, const std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
