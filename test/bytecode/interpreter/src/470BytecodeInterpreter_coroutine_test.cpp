#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"

#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

#include "BytecodeInterpreterTest.h"

#include "Runtime/Value/InterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace {
	// Native functions suspending the running coroutine, as a game "wait" or "choice" command would do
	class CoroutineTestModule : public ska::lang::Module<ska::bytecode::Interpreter> {
	public:
		CoroutineTestModule(ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>& config, std::vector<std::string>& events) :
			ska::lang::Module<ska::bytecode::Interpreter> { config, "test.coroutine", SKALANG_TEST_DIR "/src/resources/coroutine_binding" },
			m_builder(ska::BridgeBuilder<ska::bytecode::Interpreter> { m_bridge }) {
			auto& interpreter = config.interpreter;
			m_builder.bindField("wait", [&interpreter, &events](std::vector<ska::NodeValue> params) {
				events.push_back("wait " + params[0].convertString());
				interpreter.suspend();
				return ska::NodeValue{};
			});
			m_builder.bindField("choice", [&interpreter, &events](std::vector<ska::NodeValue> params) {
				events.push_back("choice " + params[0].convertString());
				interpreter.suspend();
				// Ignored : the returned value is the one given when resuming
				return ska::NodeValue{ 1000L };
			});
			m_builder.bindField("event", [&events](std::vector<ska::NodeValue> params) {
				events.push_back(params[0].convertString());
				return ska::NodeValue{};
			});
			m_builder.bindField("describe", [](std::vector<ska::NodeValue> params) {
				return ska::NodeValue{ std::make_shared<std::string>(params[0].convertString() + " " + params[1].convertString() + " " + params[2].convertString()) };
			});
			m_builder.generate();
		}
	private:
		ska::BridgeBuilder<ska::bytecode::Interpreter> m_builder;
	};

	struct CoroutineTest {
		BytecodeInterpreterDataTestContainer data;
		ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> moduleConfiguration;
		std::vector<std::string> events;
		CoroutineTestModule module;
		ska::bytecode::Executor executor;
		std::size_t scriptIndex = 0;

		explicit CoroutineTest(const std::string& program) :
			data(Prepare(program)),
			moduleConfiguration { data.storage->astCache, *data.typeBuilder, *data.typeChecker, reservedKeywords, *data.parser, *data.storage, *data.interpreter },
			module(moduleConfiguration, events) {
			readerI->parse(*data.parser);
			auto& generated = data.generator->generate(*data.storage, ska::bytecode::ScriptGenerationHelper{ *data.storage, *readerI });
			scriptIndex = generated.id();
		}

		ska::bytecode::Coroutine start(const std::string& function, std::vector<ska::NodeValue> arguments = {}) {
			auto functionValue = data.interpreter->exported(executor, scriptIndex, *data.storage, function);
			return data.interpreter->start(executor, *data.storage, functionValue, std::move(arguments));
		}

		void resume(ska::bytecode::Coroutine& coroutine, ska::NodeValue value = {}) {
			data.interpreter->resume(coroutine, std::move(value));
		}

	private:
		static BytecodeInterpreterDataTestContainer Prepare(const std::string& program) {
			auto data = BytecodeInterpreterDataTestContainer{};
			ASTFromInputBytecodeInterpreterNoParse(program, data);
			return data;
		}
	};

	std::string Joined(const std::vector<std::string>& events) {
		auto result = std::string{};
		for (const auto& event : events) {
			result += (result.empty() ? "" : ", ") + event;
		}
		return result;
	}
}

TEST_CASE("[BytecodeInterpreter] Coroutine : function called by the host, with arguments, without suspension") {
	auto test = CoroutineTest {
		"describe = function(name: string, count: int) : string do\n"
		"	return name + \" x\" + count\n"
		"end\n"
	};
	auto coroutine = test.start("describe", { ska::NodeValue{ std::make_shared<std::string>("apple") }, ska::NodeValue{ 3L } });
	CHECK(coroutine.finished());
	CHECK(*coroutine.result().nodeval<ska::StringShared>() == "apple x3");
}

TEST_CASE("[BytecodeInterpreter] Coroutine : suspended by a native function, then resumed") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"run = function(count: int) : int do\n"
		"	total = 0\n"
		"	for(i = 0\n i < count\n i = i + 1) do\n"
		"		Binding.event(\"step \" + i)\n"
		"		Binding.wait(i * 10)\n"
		"		total = total + i\n"
		"	end\n"
		"	return total\n"
		"end\n"
	};
	auto coroutine = test.start("run", { ska::NodeValue{ 3L } });
	CHECK(coroutine.suspended());
	CHECK(Joined(test.events) == "step 0, wait 0");

	test.resume(coroutine);
	CHECK(coroutine.suspended());
	CHECK(Joined(test.events) == "step 0, wait 0, step 1, wait 10");

	test.resume(coroutine);
	test.resume(coroutine);
	CHECK(coroutine.finished());
	CHECK(coroutine.result().nodeval<long>() == 3);
	CHECK(Joined(test.events) == "step 0, wait 0, step 1, wait 10, step 2, wait 20");

	CHECK_THROWS(test.resume(coroutine));
}

TEST_CASE("[BytecodeInterpreter] Coroutine : the value returned by a suspending native function is given when resuming") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"ask = function() : string do\n"
		"	answer = Binding.choice(\"left or right ?\")\n"
		"	if answer == 1\n"
		"		return \"right\"\n"
		"	end\n"
		"	return \"left \" + answer\n"
		"end\n"
	};
	auto right = test.start("ask");
	CHECK(right.suspended());
	test.resume(right, ska::NodeValue{ 1L });
	CHECK(right.finished());
	CHECK(*right.result().nodeval<ska::StringShared>() == "right");

	auto left = test.start("ask");
	test.resume(left, ska::NodeValue{ 0L });
	CHECK(*left.result().nodeval<ska::StringShared>() == "left 0");
}

TEST_CASE("[BytecodeInterpreter] Coroutine : suspension inside nested function calls and closures") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"walk = function(steps: int) : int do\n"
		"	done = 0\n"
		"	step = function() do\n"
		"		Binding.wait(done)\n"
		"		done = done + 1\n"
		"	end\n"
		"	for(i = 0\n i < steps\n i = i + 1) do\n"
		"		step()\n"
		"	end\n"
		"	return done\n"
		"end\n"
		"run = function() : int do\n"
		"	return walk(2) * 100 + walk(1)\n"
		"end\n"
	};
	auto coroutine = test.start("run");
	auto resumes = 0;
	while (coroutine.suspended()) {
		test.resume(coroutine);
		resumes++;
	}
	CHECK(resumes == 3);
	CHECK(coroutine.result().nodeval<long>() == 201);
	CHECK(Joined(test.events) == "wait 0, wait 1, wait 0");
}

TEST_CASE("[BytecodeInterpreter] Coroutine : interleaved coroutines share the script variables") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"counter = 0\n"
		"count = function(name: string, times: int) : int do\n"
		"	for(i = 0\n i < times\n i = i + 1) do\n"
		"		counter = counter + 1\n"
		"		Binding.event(name + \" \" + counter)\n"
		"		Binding.wait(0)\n"
		"	end\n"
		"	return counter\n"
		"end\n"
	};
	auto first = test.start("count", { ska::NodeValue{ std::make_shared<std::string>("a") }, ska::NodeValue{ 2L } });
	auto second = test.start("count", { ska::NodeValue{ std::make_shared<std::string>("b") }, ska::NodeValue{ 3L } });
	test.resume(first);
	test.resume(second);
	test.resume(first);
	CHECK(first.finished());
	CHECK(first.result().nodeval<long>() == 4);
	test.resume(second);
	test.resume(second);
	CHECK(second.finished());
	CHECK(second.result().nodeval<long>() == 5);
	CHECK(Joined(test.events) == "a 1, wait 0, b 2, wait 0, a 3, wait 0, b 4, wait 0, b 5, wait 0");
}

TEST_CASE("[BytecodeInterpreter] Coroutine : cannot suspend outside of a coroutine, nor in the top level code of a script") {
	SUBCASE("script top level code run directly") {
		auto test = CoroutineTest {
			"Binding = import \"bind:test.coroutine\"\n"
			"Binding.wait(1)\n"
		};
		CHECK_THROWS(test.data.interpreter->load(test.executor, test.scriptIndex, *test.data.storage));
	}
	SUBCASE("imported script top level code, imported by a coroutine") {
		auto test = CoroutineTest {
			"run = function() do\n"
			"	Imported = import \"" SKALANG_TEST_DIR "/src/resources/coroutine_top_level_wait\"\n"
			"end\n"
		};
		auto run = test.data.interpreter->exported(test.executor, test.scriptIndex, *test.data.storage, "run");
		CHECK_THROWS(test.data.interpreter->start(test.executor, *test.data.storage, run));
	}
}

TEST_CASE("[BytecodeInterpreter] Coroutine : a failed coroutine reports its error, the executor stays usable") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"fail = function(values: int[]) : int do\n"
		"	Binding.wait(0)\n"
		"	return values[10]\n"
		"end\n"
		"twice = function(value: int) : int do\n"
		"	return value * 2\n"
		"end\n"
	};
	auto failing = test.start("fail", { ska::NodeValue{ std::make_shared<ska::NodeValueArrayRaw>() } });
	CHECK(failing.suspended());
	CHECK_THROWS(test.resume(failing));
	CHECK(failing.state() == ska::bytecode::Coroutine::State::FAILED);

	auto working = test.start("twice", { ska::NodeValue{ 21L } });
	CHECK(working.result().nodeval<long>() == 42);
}

TEST_CASE("[BytecodeInterpreter] Script top level code runs once, even when imported again after a call of one of its functions") {
	auto test = CoroutineTest {
		"A = import \"" SKALANG_TEST_DIR "/src/resources/reimport_a\"\n"
		"run = function() : int do\n"
		"	first = A.f()\n"
		"	B = import \"" SKALANG_TEST_DIR "/src/resources/reimport_b\"\n"
		"	return first * 10 + B.g()\n"
		"end\n"
	};
	auto coroutine = test.start("run");
	CHECK(coroutine.finished());
	CHECK(coroutine.result().nodeval<long>() == 12);
	CHECK(Joined(test.events) == "a top level");
}

TEST_CASE("[BytecodeInterpreter] Native function : receives its parameters in the declaration order") {
	auto test = CoroutineTest {
		"Binding = import \"bind:test.coroutine\"\n"
		"describe = function() : string do\n"
		"	return Binding.describe(\"apple\", 3, true)\n"
		"end\n"
	};
	auto coroutine = test.start("describe");
	CHECK(*coroutine.result().nodeval<ska::StringShared>() == "apple 3 true");
}
