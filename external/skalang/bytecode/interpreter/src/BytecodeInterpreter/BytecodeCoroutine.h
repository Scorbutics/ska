#pragma once
#include "Runtime/Value/NodeValue.h"
#include "Runtime/Value/ScriptVariableRef.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {
		class Interpreter;
		class Executor;

		// A function call started by the host, that can be suspended (by a native function calling Interpreter::suspend)
		// and resumed later by the host (Interpreter::resume).
		// The coroutines started on an executor share its scripts and their variables : the executor must outlive them.
		class Coroutine {
			friend class Interpreter;
		public:
			enum class State {
				// Waits to be resumed
				SUSPENDED,
				// The called function returned : its returned value is available
				FINISHED,
				// An error interrupted the execution
				FAILED
			};

			Coroutine(Coroutine&&) noexcept = default;
			Coroutine& operator=(Coroutine&&) noexcept = default;
			Coroutine(const Coroutine&) = delete;
			Coroutine& operator=(const Coroutine&) = delete;
			~Coroutine() = default;

			State state() const { return m_state; }
			bool suspended() const { return m_state == State::SUSPENDED; }
			bool finished() const { return m_state == State::FINISHED; }

			// Value returned by the called function (empty for a function without return value)
			const NodeValue& result() const { return m_result; }

		private:
			Coroutine(Executor& executor, GenerationOutput& instructions) :
				m_executor(&executor),
				m_instructions(&instructions) {
			}

			Executor* m_executor;
			GenerationOutput* m_instructions;
			// Call frames and value stack of the coroutine, while it does not run
			ExecutionThread m_thread;
			// Last executed instruction (the one that suspended the coroutine)
			ScriptVariableRef m_resumeAddress;
			State m_state = State::SUSPENDED;
			NodeValue m_result;
		};
	}
}
