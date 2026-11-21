#include <doctest.h>
#include <map>
#include "Scripting/__internalConfig/LoggerConfig.h"
#include "Scripting/ScriptVM.h"
#include "Scripting/EngineModule.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/Data/MemoryScript.h"

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
		ska::scripting::EngineModule engine { vm, entityManager, locator, &savegame };

		int variable(const std::string& name) const { return savegame.getGameVariable(name); }
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
