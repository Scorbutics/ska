#pragma once

#include "Value/ScriptExecutionOutput.h"

namespace ska {
	namespace bytecode {
		class Interpreter;
		class Generator;
		class ExecutionContext;

		class InterpreterCommandUnit {
		public:
			virtual ScriptExecutionOutput interpret(ExecutionContext& node) = 0;
			virtual ~InterpreterCommandUnit() = default;
		};

		class InterpreterCommandBase :
			public InterpreterCommandUnit {
		public:
			InterpreterCommandBase(Interpreter& interpreter, Generator& generator) :
				m_interpreter(interpreter),
				m_generator(generator) {}
		protected:
			Interpreter& m_interpreter;
			Generator& m_generator;
		};
	}
}
