#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Service/ReservedKeywordsPool.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "BytecodeInterpreter/BytecodeCoroutine.h"
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "Runtime/Value/ModuleConfiguration.h"
#include "std/module/io/log.h"
#include "std/module/math.h"

#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/Basics/Script/ScriptRunner.h"

namespace ska {
	namespace scripting {

		// What a running script is about
		struct ScriptContext {
			// Script file
			std::string name;
			// Entity that triggered the script (ex : the one the player talked to)
			std::optional<EntityId> origin;
			// Entity targeted by the script, if any
			std::optional<EntityId> target;
		};

		// Runs skalang scripts for the engine.
		// A script file declares a "run" function, called with the script arguments :
		//     run = function(args: string[]) do ... end
		// A script runs until it ends, or until a native function suspends it (ex : "wait") : it is then resumed by "update",
		// once its waiting condition is satisfied. All the scripts share the same script modules (imported once).
		class ScriptVM : public ScriptRunner {
		public:
			using Modules = lang::ModuleConfiguration<bytecode::Interpreter>;

			// "standardLibraryPath" : directory of the standard library and engine bindings scripts ("std:" imports)
			explicit ScriptVM(std::string standardLibraryPath = DefaultStandardLibraryPath());
			~ScriptVM() override;
			ScriptVM(const ScriptVM&) = delete;
			ScriptVM& operator=(const ScriptVM&) = delete;

			// Default : "std" directory next to the executable
			static std::string DefaultStandardLibraryPath();

			// Configuration to give to the binding modules (lang::Module)
			Modules& modules() { return m_modules; }

			// Starts the "run" function of a script file. When a script is running, the start is done after its current step.
			// Returns false when the script could not be started (compilation or execution error : logged).
			bool start(const std::string& scriptFile, std::vector<std::string> arguments = {}, ScriptContext context = {});

			// Resumes the suspended scripts that are ready to continue
			void update(unsigned int elapsedTime) override;

			// Starts a script triggered by the engine (ScriptRunner) : not started when the same script file is still running
			// (ex : the action key kept pressed, a block walked on again during the script)
			void start(const ScriptSleepComponent& script, EntityId origin) override;
			bool running(const std::string& scriptFile, const std::vector<std::string>& arguments) const;

			// Number of suspended scripts
			std::size_t suspendedScripts() const { return m_suspended.size(); }

			// Time elapsed (sum of the "update" elapsed times), in milliseconds
			unsigned long time() const { return m_time; }

			/* For the native functions */

			// Context of the running script (empty when no script is running)
			const ScriptContext& context() const;

			// Suspends the running script until "ready" is true (checked at each update).
			// "value" gives the value returned to the script by the suspending native function, when it has one.
			void suspendUntil(std::function<bool()> ready, std::function<NodeValue()> value = {});
			// Suspends the running script for a duration (in milliseconds)
			void wait(unsigned int milliseconds);

		private:
			static std::string Key(const std::string& scriptFile, const std::vector<std::string>& arguments);

			struct RunningScript {
				bytecode::Coroutine coroutine;
				ScriptContext context;
				std::string key;
				std::function<bool()> ready;
				std::function<NodeValue()> value;
			};
			struct PendingStart {
				std::string scriptFile;
				std::vector<std::string> arguments;
				ScriptContext context;
			};
			struct Wait {
				std::function<bool()> ready;
				std::function<NodeValue()> value;
			};

			std::optional<std::size_t> compile(const std::string& scriptFile);
			bool startNow(PendingStart script);
			// Records the waiting condition of a coroutine that just ran : returns false when it is not suspended anymore
			bool afterStep(RunningScript& script);
			void startPendingScripts();
			static Modules& WithStandardLibraryPath(Modules& modules, std::string standardLibraryPath);

			// Runs a step of a script (start or resume) : the native functions it calls see its context
			template <class Step>
			auto runStep(const ScriptContext& context, Step&& step) {
				struct RunningGuard {
					ScriptVM& vm;
					RunningGuard(ScriptVM& vm, const ScriptContext& context) : vm(vm) { vm.m_scriptRunning = true; vm.m_runningContext = context; }
					~RunningGuard() { vm.m_scriptRunning = false; vm.m_runningContext = {}; }
				} guard { *this, context };
				return step();
			}

			ReservedKeywordsPool m_reservedKeywords;
			TypeCrosser m_typeCrosser;
			StatementParser m_parser;
			TypeBuilder m_typeBuilder;
			SemanticTypeChecker m_typeChecker;
			bytecode::ScriptCache m_scriptCache;
			bytecode::Generator m_generator;
			bytecode::Interpreter m_interpreter;
			Modules m_modules;
			// The executor outlives the coroutines started on it
			bytecode::Executor m_executor;
			lang::IOLogModule<bytecode::Interpreter> m_logModule;
			lang::MathModule<bytecode::Interpreter> m_mathModule;

			std::unordered_map<std::string, std::size_t> m_compiledScripts;
			std::unordered_set<std::string> m_failedScripts;
			std::vector<std::unique_ptr<RunningScript>> m_suspended;
			std::vector<PendingStart> m_pendingStarts;
			bool m_scriptRunning = false;
			ScriptContext m_runningContext;
			std::optional<Wait> m_wait;
			unsigned long m_time = 0;
		};
	}
}
