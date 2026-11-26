#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

// Reference cycles (closures capturing each other through cells) are freed by the cycle collector,
// periodically during the interpretation and when the interpretation state is released.

namespace {
	// "isEven" and "isOdd" call each other through the captured variable "next" : a reference cycle of closures and cells
	constexpr auto MutualRecursionFactory = R"script(
		Make = function(): var do
			next = function(i: int): bool do
				return false
			end
			isEven = function(i: int): bool do
				r = true
				if i > 0
					r = next(i - 1)
				end
				return r
			end
			isOdd = function(i: int): bool do
				r = false
				if i > 0
					r = isEven(i - 1)
				end
				return r
			end
			next = isOdd
			return {
				even = isEven
			}
		end
	)script";
}

TEST_CASE("[BytecodeInterpreter] cycle collector : unreachable cycles are collected, reachable ones are kept") {
	const auto progStr = std::string{ MutualRecursionFactory } + R"script(
		kept = Make()
		total = 0
		for(i = 0
		i < 3000
		i = i + 1) do
			garbage = Make()
			if garbage.even(4)
				total = total + 1
			end
			if kept.even(6)
				total = total + 1
			end
			if kept.even(3)
				total = total + 100
			end
		end
		out = total
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto executor = data.interpreter->interpret(gen.id(), *data.storage);

	// "kept" is used after each periodic collection : a wrong collection would have broken its cycle
	CHECK(executor->variable(gen.id()).nodeval<long>() == 6000);

	// Periodic collections freed the garbage cycles during the interpretation : only the ones created since the last
	// collection are left (each loop iteration creates a garbage cycle, 3000 iterations in total)
	const auto garbageLeft = executor->collector().collect();
	CHECK(garbageLeft > 0);
	CHECK(garbageLeft < 1000);
	CHECK(executor->collector().collect() == 0);
}

TEST_CASE("[BytecodeInterpreter] cycle collector : a collection does not alter reachable values") {
	const auto progStr = std::string{ MutualRecursionFactory } + R"script(
		kept = Make()
		out = kept
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto executor = data.interpreter->interpret(gen.id(), *data.storage);

	// The cycle of "kept" is reachable from the script variables : nothing to collect
	CHECK(executor->collector().collect() == 0);
	const auto kept = executor->variable(gen.id());
	CHECK(kept.nodeval<ska::NodeValueArray>()->size() == 1);
}

TEST_CASE("[BytecodeInterpreter] cycle collector : a map referenced by a closure it contains is collected") {
	const auto progStr = std::string{ R"script(
		Callback = function(): int do return 0 end
		Make = function(): int do
			callbacks = [:]: Callback
			callbacks["size"] = function(): int do
				return callbacks.size()
			end
			return callbacks["size"]()
		end
		total = 0
		for(i = 0
		i < 10
		i = i + 1) do
			total = total + Make()
		end
		out = total
	)script" };

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto executor = data.interpreter->interpret(gen.id(), *data.storage);
	CHECK(executor->variable(gen.id()).nodeval<long>() == 10);
	// Each call left a cycle : the map, the closure, the cell of "callbacks"
	CHECK(executor->collector().collect() > 0);
	CHECK(executor->collector().collect() == 0);
}
