#pragma once
#include "ControlStatement.h"

namespace ska {
	class CommandIf : 
		public ControlStatement {
	public:
		explicit CommandIf(EntityManager& e);
		void operator=(const CommandIf&) = delete;
		virtual ~CommandIf() = default;

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, const std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
