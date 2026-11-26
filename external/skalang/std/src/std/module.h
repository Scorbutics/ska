#pragma once

#include "Runtime/Service/ScriptBinding.h"
#include "Runtime/Value/ModuleConfiguration.h"

namespace ska {

	namespace lang {
    	template <class Interpreter>
    	class Module {
    	public:
        	Module(ModuleConfiguration<Interpreter>& config, std::string moduleName, std::string templateScriptName):
            	m_bridge{ config, std::move(moduleName), std::move(templateScriptName) } {
        	}

        	virtual ~Module() = default;

    	protected:
        	ScriptBinding<Interpreter> m_bridge;
    	};
	}
}
