#include <doctest.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <unistd.h>
#include "Scripting/__internalConfig/LoggerConfig.h"
#include "Scripting/ScriptVM.h"
#include "Scripting/EngineModule.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/Data/MemoryScript.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace {
	class MemoryScriptTest : public ska::MemoryScript {
	public:
		std::string getSaveName() const override { return "test"; }
		int getGameVariable(const std::string& var) const override {
			auto value = variables.find(var);
			return value == variables.end() ? 0 : value->second;
		}
		void setGameVariable(const std::string& var, int value) override { variables[var] = value; }
		std::map<std::string, int> variables;
	};

	std::string Script(const std::string& name) {
		return SKA_SCRIPTING_TEST_DIR "/scripts/" + name + ".miniska";
	}

	struct ScriptingTest {
		ska::GameEventDispatcher ged;
		ska::EntityManager entityManager { ged };
		ska::EntityLocator locator { ged };
		MemoryScriptTest savegame;
		ska::scripting::ScriptVM vm;
		ska::scripting::EntityHandles handles { ged };
		ska::scripting::EngineModule engine { vm, entityManager, locator, handles, &savegame };

		int variable(const std::string& name) const { return savegame.getGameVariable(name); }
	};

	// Test native module : emits an engine event (as a game function would, during a script step)
	class EmitterModule : public ska::lang::Module<ska::bytecode::Interpreter> {
	public:
		EmitterModule(ska::scripting::ScriptVM& vm, ska::scripting::EngineModule& engine) :
			ska::lang::Module<ska::bytecode::Interpreter> { vm.modules(), "test.emitter", SKA_SCRIPTING_TEST_DIR "/scripts/test.emitter" },
			m_builder(ska::BridgeBuilder<ska::bytecode::Interpreter> { m_bridge }) {
			m_builder.bindField("emit", [&engine](std::vector<ska::NodeValue> params) {
				engine.emit(*params[0].nodeval<ska::StringShared>(), std::optional<ska::EntityId>{});
				return ska::NodeValue{};
			});
			m_builder.generate();
		}
	private:
		ska::BridgeBuilder<ska::bytecode::Interpreter> m_builder;
	};
}

TEST_CASE("[ScriptVM] runs the \"run\" function of a script, with its arguments and its context") {
	auto test = ScriptingTest{};
	auto context = ska::scripting::ScriptContext{};
	context.origin = 7;
	CHECK(test.vm.start(Script("basic"), { "a", "b" }, context));
	CHECK(test.variable("arguments") == 2);
	CHECK(test.variable("origin") == 7);
	CHECK(test.vm.suspendedScripts() == 0);

	SUBCASE("a script can be run again") {
		CHECK(test.vm.start(Script("basic"), { "a" }));
		CHECK(test.variable("arguments") == 1);
		CHECK(test.variable("origin") == -1);
	}
}

TEST_CASE("[ScriptVM] wait : the script is resumed once the time elapsed") {
	auto test = ScriptingTest{};
	CHECK(test.vm.start(Script("wait")));
	CHECK(test.variable("step") == 1);
	CHECK(test.vm.suspendedScripts() == 1);

	test.vm.update(60);
	CHECK(test.variable("step") == 1);
	test.vm.update(40);
	CHECK(test.variable("step") == 2);
	test.vm.update(49);
	CHECK(test.variable("step") == 2);
	test.vm.update(1);
	CHECK(test.variable("step") == 3);
	CHECK(test.vm.suspendedScripts() == 0);
}

TEST_CASE("[ScriptVM] scripts waiting at the same time") {
	auto test = ScriptingTest{};
	CHECK(test.vm.start(Script("wait")));
	test.vm.update(100);
	CHECK(test.vm.start(Script("wait")));
	CHECK(test.vm.suspendedScripts() == 2);
	// The first one ends, the second one goes on
	test.vm.update(50);
	CHECK(test.vm.suspendedScripts() == 1);
	test.vm.update(50);
	test.vm.update(50);
	CHECK(test.vm.suspendedScripts() == 0);
	CHECK(test.variable("step") == 3);
}

TEST_CASE("[ScriptVM] entities : found by name, components disabled then restored as they were") {
	auto test = ScriptingTest{};
	const auto hero = test.entityManager.createEntity();
	auto input = ska::InputComponent{};
	input.movePower = 12.5F;
	test.entityManager.addComponent<ska::InputComponent>(hero, std::move(input));

	SUBCASE("unknown entity") {
		CHECK(test.vm.start(Script("components")));
		CHECK(test.variable("found") == 0);
	}

	SUBCASE("named entity") {
		test.entityManager.setName(hero, "hero");
		CHECK(test.vm.start(Script("components")));
		CHECK(test.variable("found") == 1);
		CHECK(!test.entityManager.hasComponent<ska::InputComponent>(hero));
		test.vm.update(10);
		REQUIRE(test.entityManager.hasComponent<ska::InputComponent>(hero));
		CHECK(test.entityManager.getComponent<ska::InputComponent>(hero).movePower == doctest::Approx(12.5F));
	}
}

TEST_CASE("[ScriptVM] a script starts another script, after its own step") {
	auto test = ScriptingTest{};
	CHECK(test.vm.start(Script("starter"), { Script("basic") }));
	CHECK(test.variable("starter") == 1);
	CHECK(test.variable("arguments") == 3);
}

TEST_CASE("[ScriptVM] errors : a broken script is not started, a failing script is stopped") {
	auto test = ScriptingTest{};
	CHECK(!test.vm.start(Script("broken")));
	// Still not started (and no crash) the next times
	CHECK(!test.vm.start(Script("broken")));
	CHECK(!test.vm.start(Script("does_not_exist")));

	CHECK(test.vm.start(Script("failing"), { "a" }));
	CHECK(test.vm.suspendedScripts() == 1);
	test.vm.update(10);
	CHECK(test.variable("failing") == 1);
	CHECK(test.vm.suspendedScripts() == 0);

	// The other scripts still run
	CHECK(test.vm.start(Script("wait")));
	test.vm.update(100);
	test.vm.update(50);
	CHECK(test.variable("step") == 3);
}

TEST_CASE("[ScriptVM] started by the engine triggers : not started again while still running") {
	auto test = ScriptingTest{};
	auto script = ska::ScriptSleepComponent{};
	script.name = Script("wait");
	ska::ScriptRunner& runner = test.vm;

	runner.start(script, 3);
	runner.start(script, 3);
	CHECK(test.vm.suspendedScripts() == 1);

	// Other arguments (ex : the player walked on the block from another position) : still the same running script
	script.args.push_back("other");
	runner.start(script, 3);
	CHECK(test.vm.suspendedScripts() == 1);

	runner.update(100);
	runner.update(50);
	CHECK(test.vm.suspendedScripts() == 0);
	// Done : it can be started again
	script.args.clear();
	runner.start(script, 3);
	CHECK(test.vm.suspendedScripts() == 1);
}

TEST_CASE("[ScriptVM] the standard library modules are available (math)") {
	auto test = ScriptingTest{};
	CHECK(test.vm.start(Script("math")));
	CHECK(test.variable("math") == 73);
}

TEST_CASE("[ScriptVM] synchronous calls : the engine gets the value of a script function right away") {
	auto test = ScriptingTest{};
	const auto calls = Script("calls");
	CHECK(test.vm.call<long>(calls, "damage", { ska::NodeValue{ 10L }, ska::NodeValue{ 5L } }) == 15);
	CHECK(test.vm.call<long>(calls, "damage", { ska::NodeValue{ 1L }, ska::NodeValue{ 5L } }) == 1);
	CHECK(test.vm.call<double>(calls, "ratio", { ska::NodeValue{ 3.0 }, ska::NodeValue{ 2.0 } }) == doctest::Approx(1.5));
	CHECK(test.vm.call<std::string>(calls, "describe", { ska::NodeValue{ std::make_shared<std::string>("pikachu") }, ska::NodeValue{ 12L } }) == "pikachu 12");
	CHECK(test.vm.call<bool>(calls, "lucky", { ska::NodeValue{ 4L } }));
	CHECK(!test.vm.call<bool>(calls, "lucky", { ska::NodeValue{ 3L } }));
	test.vm.call<void>(calls, "notify", { ska::NodeValue{ std::make_shared<std::string>("notified") } });
	CHECK(test.variable("notified") == 1);
	CHECK(test.vm.suspendedScripts() == 0);

	SUBCASE("a script started by the function starts right after the call") {
		CHECK(test.vm.call<long>(calls, "startsScript", { ska::NodeValue{ std::make_shared<std::string>(Script("basic")) } }) == 1);
		CHECK(test.variable("arguments") == 1);
	}
}

TEST_CASE("[ScriptVM] synchronous calls : checked against the function type") {
	auto test = ScriptingTest{};
	const auto calls = Script("calls");
	CHECK_THROWS_WITH(test.vm.call<long>(calls, "missing"), doctest::Contains("has no function named \"missing\""));
	CHECK_THROWS_WITH(test.vm.call<long>(calls, "damage", { ska::NodeValue{ 10L } }), doctest::Contains("with 1 argument(s)"));
	CHECK_THROWS_WITH(test.vm.call<long>(calls, "damage", { ska::NodeValue{ 10L }, ska::NodeValue{ 5.0 } }), doctest::Contains("the argument 2 is not a int"));
	CHECK_THROWS_WITH(test.vm.call<std::string>(calls, "damage", { ska::NodeValue{ 10L }, ska::NodeValue{ 5L } }), doctest::Contains("expects a string returned value"));
	CHECK_THROWS_WITH(test.vm.call<long>(Script("broken"), "run"), doctest::Contains("could not be compiled"));
}

TEST_CASE("[ScriptVM] synchronous calls : a failing or waiting function is an error, the VM stays usable") {
	auto test = ScriptingTest{};
	const auto calls = Script("calls");
	CHECK_THROWS_WITH(test.vm.call<long>(calls, "divide", { ska::NodeValue{ 1L }, ska::NodeValue{ 0L } }), doctest::Contains("division by zero"));
	CHECK_THROWS_WITH(test.vm.call<long>(calls, "waits"), doctest::Contains("cannot wait"));
	CHECK(test.vm.suspendedScripts() == 0);
	CHECK(test.vm.call<long>(calls, "divide", { ska::NodeValue{ 9L }, ska::NodeValue{ 3L } }) == 3);
	CHECK(test.vm.start(Script("wait")));
	CHECK(test.vm.suspendedScripts() == 1);
}

TEST_CASE("[ScriptVM] hooks : a script function called at each event, as a script (it can wait)") {
	auto test = ScriptingTest{};
	test.vm.declareEvent("test.event");
	CHECK(test.vm.start(Script("hooks")));
	CHECK(test.vm.hooks("test.event") == 1);
	CHECK(test.variable("hook") > 0);

	const auto entity = test.entityManager.createEntity();
	CHECK(test.engine.emit("test.event", entity) == 1);
	CHECK(test.variable("entity") == static_cast<int>(test.handles.handle(entity)));
	CHECK(test.vm.suspendedScripts() == 1);
	CHECK(test.engine.emit("test.event", std::optional<ska::EntityId>{}) == 1);
	CHECK(test.variable("entity") == -1);
	test.vm.update(10);
	CHECK(test.variable("count") == 2);
	CHECK(test.vm.suspendedScripts() == 0);

	SUBCASE("a hook is removed by its id") {
		CHECK(test.vm.start(Script("unhook")));
		CHECK(test.vm.hooks("test.event") == 0);
		CHECK(test.engine.emit("test.event", entity) == 0);
	}

	SUBCASE("the host clears the hooks") {
		test.vm.clearHooks();
		CHECK(test.engine.emit("test.event", entity) == 0);
	}
}

TEST_CASE("[ScriptVM] hooks : emitted during a script step, they start after it") {
	auto test = ScriptingTest{};
	auto emitter = EmitterModule{ test.vm, test.engine };
	test.vm.declareEvent("test.event");
	CHECK(test.vm.start(Script("hooks")));
	CHECK(test.vm.start(Script("emitter")));
	// The hook had not run yet when the emitter read the count
	CHECK(test.variable("emitted") == 100);
	CHECK(test.variable("entity") == -1);
	test.vm.update(10);
	CHECK(test.variable("count") == 1);
}

TEST_CASE("[ScriptVM] hooks : an unknown event is an error") {
	auto test = ScriptingTest{};
	test.vm.declareEvent("test.event");
	CHECK(!test.vm.start(Script("unknown_event")));
	CHECK(test.variable("registered") == 0);
	CHECK_THROWS_WITH(test.vm.emit("test.unknown"), doctest::Contains("unknown event"));
}

TEST_CASE("[ScriptVM] entity handles : a handle of a removed entity is not valid anymore") {
	auto test = ScriptingTest{};
	const auto hero = test.entityManager.createEntity();
	test.entityManager.setName(hero, "hero");
	test.entityManager.addComponent<ska::InputComponent>(hero, ska::InputComponent{});
	CHECK(test.handles.handle(hero) == static_cast<long>(hero));
	CHECK(test.vm.start(Script("handles")));
	CHECK(test.variable("handle") == static_cast<int>(hero));

	SUBCASE("the entity still exists") {
		test.vm.update(10);
		CHECK(test.variable("exists") == 1);
		CHECK(test.variable("used") == 1);
		CHECK(!test.entityManager.hasComponent<ska::InputComponent>(hero));
	}

	SUBCASE("the entity was removed, and its id reused") {
		test.entityManager.removeEntity(hero);
		const auto other = test.entityManager.createEntity();
		REQUIRE(other == hero);
		test.entityManager.addComponent<ska::InputComponent>(other, ska::InputComponent{});
		CHECK(test.handles.handle(other) != static_cast<long>(hero));
		CHECK(!test.handles.find(static_cast<long>(hero)).has_value());
		CHECK(test.handles.find(test.handles.handle(other)) == other);

		test.vm.update(10);
		CHECK(test.variable("exists") == 0);
		// The script stopped with an error before acting on the new entity
		CHECK(test.variable("used") == 0);
		CHECK(test.entityManager.hasComponent<ska::InputComponent>(other));
		CHECK(test.vm.suspendedScripts() == 0);
	}

	SUBCASE("invalid handles") {
		CHECK(!test.handles.find(-1).has_value());
		CHECK_THROWS_WITH(test.handles.entity(-1), doctest::Contains("no entity"));
		CHECK_THROWS(test.handles.entity(static_cast<long>(hero) + 65536L));
	}
}

/* Development console : the ScriptVM is the evaluator of "eval" and "run" (tools/ska-console) */

namespace {
	struct ConsoleAnswer {
		bool answered = false;
		ska::DevReply reply;
	};
	ska::DevReplier Answer(ConsoleAnswer& answer) {
		return [&answer](ska::DevReply reply) { answer.answered = true; answer.reply = std::move(reply); };
	}
}

TEST_CASE("[ScriptVM] console eval : an expression answers its value, with the engine functions") {
	auto test = ScriptingTest{};
	test.savegame.setGameVariable("step", 3);
	auto answer = ConsoleAnswer{};
	test.vm.eval("Engine.variable(\"step\") + 1", Answer(answer));
	CHECK(answer.answered);
	CHECK(answer.reply.ok);
	CHECK(answer.reply.value == "4");
}

TEST_CASE("[ScriptVM] console eval : statements that wait answer once they are over") {
	auto test = ScriptingTest{};
	auto answer = ConsoleAnswer{};
	test.vm.eval("Engine.wait(100)\nEngine.setVariable(\"done\", 1)\nDev.reply(\"done\")", Answer(answer));
	CHECK(!answer.answered);
	test.vm.update(60);
	CHECK(!answer.answered);
	test.vm.update(60);
	CHECK(answer.answered);
	CHECK(answer.reply.ok);
	CHECK(answer.reply.value == "done");
	CHECK(test.variable("done") == 1);
}

TEST_CASE("[ScriptVM] console eval : a code that does not compile, or that fails, answers the error") {
	auto test = ScriptingTest{};
	auto answer = ConsoleAnswer{};
	test.vm.eval("Nope.call()", Answer(answer));
	CHECK(answer.answered);
	CHECK(!answer.reply.ok);
	CHECK(answer.reply.error.find("Nope") != std::string::npos);

	auto failed = ConsoleAnswer{};
	test.vm.eval("a = [1]\nb = a[5]", Answer(failed));
	CHECK(failed.answered);
	CHECK(!failed.reply.ok);
}

TEST_CASE("[ScriptVM] console run : a script file with its arguments, answered by Dev.reply") {
	auto test = ScriptingTest{};
	auto answer = ConsoleAnswer{};
	test.vm.run(Script("console_reply"), { "a", "b" }, Answer(answer));
	CHECK(answer.answered);
	CHECK(answer.reply.ok);
	CHECK(answer.reply.value == "2 arguments\na");

	auto missing = ConsoleAnswer{};
	test.vm.run(Script("no_such_script"), {}, Answer(missing));
	CHECK(missing.answered);
	CHECK(!missing.reply.ok);
}

TEST_CASE("[ScriptVM] console run : a script file changed since its last run is compiled again") {
	auto test = ScriptingTest{};
	const auto directory = std::filesystem::temp_directory_path() / ("ska-console-run-" + std::to_string(::getpid()));
	std::filesystem::create_directories(directory);
	const auto file = directory / "edited.miniska";
	auto write = [&file](const std::string& body, int seconds) {
		std::ofstream{ file } << body;
		// A later write (the file systems do not always tell apart two writes in the same second)
		std::filesystem::last_write_time(file, std::filesystem::file_time_type::clock::now() + std::chrono::seconds(seconds));
	};
	std::ofstream{ directory / "edited_helper.miniska" } << "word = function() : string do return \"helper\" end\n";
	auto run = [&test, &file]() {
		auto answer = ConsoleAnswer{};
		test.vm.run(file.string(), {}, Answer(answer));
		CHECK(answer.answered);
		return answer.reply;
	};

	write("Dev = import \"bind:ska.dev\"\nrun = function(args: string[]) do\nDev.reply(\"first\")\nend\n", 1);
	CHECK(run().value == "first");
	CHECK(run().value == "first");

	SUBCASE("edited : the new version, its relative imports still found") {
		write("Dev = import \"bind:ska.dev\"\nHelper = import \"edited_helper\"\nrun = function(args: string[]) do\nDev.reply(\"second \" + Helper.word())\nend\n", 2);
		CHECK(run().value == "second helper");
	}

	SUBCASE("a version that does not compile, then fixed") {
		write("run = function(args: string[]) do\nNope.call()\nend\n", 2);
		const auto broken = run();
		CHECK(!broken.ok);
		CHECK(broken.error.find("Nope") != std::string::npos);
		write("Dev = import \"bind:ska.dev\"\nrun = function(args: string[]) do\nDev.reply(\"fixed\")\nend\n", 3);
		CHECK(run().value == "fixed");
	}
	std::filesystem::remove_all(directory);
}
