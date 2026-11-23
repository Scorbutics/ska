#pragma once
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

namespace ska {
	namespace scripting {
		class ScriptVM;

		// The development console verbs for the scripts : Dev = import "bind:ska.dev" (see the "ska.dev" script template).
		// Made for the scripts run by tools/ska-console (tools/agent/*.miniska) ; without the console (SKA_DEV_CONSOLE, SKA_CONSOLE),
		// "shot" answers "" and the clock functions do nothing.
		class DevModule : public lang::Module<bytecode::Interpreter> {
		public:
			explicit DevModule(ScriptVM& vm);
			~DevModule() override = default;

		private:
			ScriptVM& m_vm;
			BridgeBuilder<bytecode::Interpreter> m_builder;
		};
	}
}
