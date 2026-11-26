#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "InterpreterCommandUnit.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeExecutionContext.h"
#include "BytecodeCoroutine.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;
	class StatementParser;
	namespace bytecode {
		class Generator;
		class Script;

		struct InterpretationPart {
			Script& script;
			ASTNode& node;
		};

		class Interpreter {
		public:
			using CommandInterpreter = std::vector<std::unique_ptr<InterpreterCommandUnit>>;

			Interpreter(StatementParser& parser, Generator& generator, const ReservedKeywordsPool& reserved);
			~Interpreter() = default;

			// Runs the top level code of the script of the context (once : nothing is done if it was already run)
			void interpret(ExecutionContext& node);
			Generator& generator() { return m_generator; }
			[[nodiscard]]
			std::unique_ptr<Executor> interpret(std::size_t scriptIndex, GenerationOutput& scripts);

			/* Host API */

			// Runs the top level code of a script in the executor (if not already done) and returns its exported values
			ScriptExecutionOutput load(Executor& executor, std::size_t scriptIndex, GenerationOutput& scripts);
			// Value exported by a script under the given name (the script is loaded if needed)
			NodeValue exported(Executor& executor, std::size_t scriptIndex, GenerationOutput& scripts, const std::string& name);

			// Calls a script function value (as a coroutine), with its arguments in the declaration order.
			// The call runs until the function returns or until it is suspended.
			Coroutine start(Executor& executor, GenerationOutput& scripts, const NodeValue& function, std::vector<NodeValue> arguments = {});
			// Continues a suspended coroutine until the called function returns or until it is suspended again.
			// "value" is the value returned by the native function that suspended the coroutine (when it has one).
			void resume(Coroutine& coroutine, NodeValue value = {});

			// Called by a native function : suspends the running coroutine once the native function returned.
			// The value returned by the native function is ignored : the one given to "resume" is used instead.
			// A coroutine cannot be suspended while running the top level code of a script (import).
			void suspend();
			bool suspensionRequested() const { return m_suspensionRequested; }

		private:
			enum class RunResult { FINISHED, SUSPENDED };
			// Executes the instructions from the current one ("advance" : from the next one) until the end or a suspension
			RunResult run(ExecutionContext& context, bool advance);
			void runCoroutine(Coroutine& coroutine, NodeValue resumeValue, bool resuming);

			CommandInterpreter build(Generator&);
			Coroutine* m_runningCoroutine = nullptr;
			// Number of script top level codes running inside the running coroutine (imports)
			std::size_t m_nestedTopLevels = 0;
			bool m_suspensionRequested = false;
			StatementParser& m_parser;
			Generator& m_generator;
			CommandInterpreter m_commandInterpreter;
		};
	}
}
