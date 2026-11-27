#pragma once
#include <filesystem>
#include <functional>
#include <type_traits>
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

#include "NodeValue/ExpressionType.h"
#include "NodeValue/Symbol.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/Basics/Script/ScriptRunner.h"
#include "Core/Dev/DevConsole.h"

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
			// Text answered to the development console (Dev.reply), for a script started by the console
			std::shared_ptr<std::string> reply;
		};

		class DevModule;

		// Runs skalang scripts for the engine.
		// A script file declares a "run" function, called with the script arguments :
		//     run = function(args: string[]) do ... end
		// A script runs until it ends, or until a native function suspends it (ex : "wait") : it is then resumed by "update",
		// once its waiting condition is satisfied. All the scripts share the same script modules (imported once).
		// It is also the evaluator of the development console (DevConsole, tools/ska-console) : "eval" and "run".
		class ScriptVM : public ScriptRunner, public DevConsoleEvaluator {
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

			/* Development console (tools/ska-console) */

			// When a started script ends : "ok" false with the error, or true with the text of the value returned by "run"
			using EndCallback = std::function<void(bool ok, const std::string& text)>;
			// Starts a script as "start", and calls "ended" when it is over (at once when it does not wait)
			bool start(const std::string& scriptFile, std::vector<std::string> arguments, ScriptContext context, EndCallback ended);
			// A line put before the code sent to the console (ex : Engine = import "bind:ska.engine") : the modules of the game
			void addConsolePrelude(std::string line);
			void eval(const std::string& code, DevReplier reply) override;
			// A script file changed since its last run is compiled again (the compiled scripts are kept by their name :
			// a new version gets a new name, "<path>~<version>", in the same directory for its relative imports)
			void run(const std::string& file, std::vector<std::string> arguments, DevReplier reply) override;

			/* Synchronous calls : the engine asks a script for a value (ex : a formula, a decision) */

			// Calls an exported function of a script file and returns its value, right away.
			// The arguments and the returned type are checked against the declared function type.
			// Throws when the script does not compile, when the function is missing or has another type, when it fails,
			// when it tries to wait (a synchronously called function cannot be suspended), or when a script is running
			// (the engine calls the scripts between their steps).
			template <class Returned>
			Returned call(const std::string& scriptFile, const std::string& function, std::vector<NodeValue> arguments = {}) {
				auto value = callFunction(scriptFile, function, std::move(arguments), ReturnedType<Returned>::type);
				if constexpr (std::is_same_v<Returned, void>) {
					return;
				} else if constexpr (std::is_same_v<Returned, std::string>) {
					return *value.template nodeval<StringShared>();
				} else {
					return value.template nodeval<Returned>();
				}
			}

			/* Hooks : the scripts react to engine events */

			// Makes an event available to the hooks (an unknown event is an error, to find the mistyped names)
			void declareEvent(std::string event);
			bool eventDeclared(const std::string& event) const;
			// Calls "function" (a script function value) each time the event is emitted. Returns the hook id.
			std::size_t addHook(const std::string& event, NodeValue function);
			bool removeHook(std::size_t hook);
			void clearHooks();
			std::size_t hooks(const std::string& event) const;
			// Starts the functions hooked on the event, as scripts (they can wait), with the arguments.
			// During a script step, they start after it. Returns the number of hooks started.
			std::size_t emit(const std::string& event, std::vector<NodeValue> arguments = {}, ScriptContext context = {});

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
			template <class T> struct ReturnedType;

			NodeValue callFunction(const std::string& scriptFile, const std::string& function, std::vector<NodeValue> arguments, ExpressionType returned);
			const Symbol* exportedSymbol(std::size_t scriptIndex, const std::string& name);

			static std::string Key(const std::string& scriptFile, const std::vector<std::string>& arguments);

			struct RunningScript {
				bytecode::Coroutine coroutine;
				ScriptContext context;
				std::string key;
				std::function<bool()> ready;
				std::function<NodeValue()> value;
				EndCallback ended;
				bool failed = false;
			};
			// A script file to run (its "run" function with string arguments), or a function value (hook) with its arguments
			struct PendingStart {
				std::string scriptFile;
				std::vector<std::string> arguments;
				ScriptContext context;
				NodeValue function;
				std::vector<NodeValue> values;
				EndCallback ended;
			};
			struct Hook {
				std::size_t id;
				std::string event;
				NodeValue function;
			};
			struct Wait {
				std::function<bool()> ready;
				std::function<NodeValue()> value;
			};

			std::optional<std::size_t> compile(const std::string& scriptFile, bool quiet = false);
			std::string writeConsoleScript(const std::string& body);
			std::string endText(RunningScript& script);
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
			// The script files run by the console : when they were written, the version compiled. The source file of
			// each new version (its name is not a file)
			struct ConsoleRun {
				std::filesystem::file_time_type written;
				unsigned int version = 0;
			};
			std::unordered_map<std::string, ConsoleRun> m_consoleRuns;
			std::unordered_map<std::string, std::string> m_sources;
			std::vector<std::unique_ptr<RunningScript>> m_suspended;
			std::vector<PendingStart> m_pendingStarts;
			bool m_scriptRunning = false;
			ScriptContext m_runningContext;
			std::optional<Wait> m_wait;
			unsigned long m_time = 0;
			std::vector<Hook> m_hooks;
			std::unordered_set<std::string> m_events;
			std::size_t m_nextHook = 1;
			std::string m_lastCompileError;
			std::vector<std::string> m_consolePrelude;
			std::size_t m_consoleScripts = 0;
			std::unique_ptr<DevModule> m_devModule;
		};

		template <> struct ScriptVM::ReturnedType<void> { static constexpr auto type = ExpressionType::VOID; };
		template <> struct ScriptVM::ReturnedType<long> { static constexpr auto type = ExpressionType::INT; };
		template <> struct ScriptVM::ReturnedType<double> { static constexpr auto type = ExpressionType::FLOAT; };
		template <> struct ScriptVM::ReturnedType<bool> { static constexpr auto type = ExpressionType::BOOLEAN; };
		template <> struct ScriptVM::ReturnedType<std::string> { static constexpr auto type = ExpressionType::STRING; };
	}
}
