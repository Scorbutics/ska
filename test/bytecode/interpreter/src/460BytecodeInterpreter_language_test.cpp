#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

namespace {
	ska::NodeValue Run(const std::string& progStr) {
		auto [script, data] = Interpret(progStr);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		return data.interpreter->interpret(gen.id(), *data.storage)->variable(gen.id());
	}
}

TEST_CASE("[BytecodeInterpreter] function compatibility : an array of compatible functions is accepted") {
	// The functions of the array only need the same parameters and return types as "Default" (their names do not matter)
	const auto result = Run(R"script(
		Default = function(value: int): int do
			return 0
		end
		applyAll = function(functions: Default[], value: int): int do
			total = 0
			functions | (f) do
				total = total + f(value)
			end
			return total
		end
		double = function(v: int): int do
			return v * 2
		end
		square = function(v: int): int do
			return v * v
		end
		out = applyAll([double, square], 3)
	)script");
	CHECK(result.nodeval<long>() == 15);
}

TEST_CASE("[BytecodeInterpreter] function compatibility : an array of incompatible functions is rejected") {
	constexpr auto progStr = R"script(
		Default = function(value: int): int do
			return 0
		end
		applyAll = function(functions: Default[]) do
		end
		toText = function(v: string): string do
			return v
		end
		applyAll([toText])
	)script";
	CHECK_THROWS_WITH(Run(progStr), doctest::Contains("Unable to use operator \"=\""));
}

TEST_CASE("[BytecodeInterpreter] syntax : an imported script does not need a final line break") {
	const auto result = Run(
		"Imported = import \"" SKALANG_TEST_DIR "/src/resources/no_final_line_break\"\n"
		"out = Imported.value\n");
	CHECK(result.nodeval<long>() == 42);
}

TEST_CASE("[BytecodeInterpreter] syntax : optional \"do\" after an if condition") {
	const auto result = Run(R"script(
		total = 0
		if 1 > 0 do
			total = total + 1
		end
		if 1 > 2 do
			total = total + 10
		else
			total = total + 100
		end
		if total == 101 do total = total + 1000 end
		out = total
	)script");
	CHECK(result.nodeval<long>() == 1101);
}

TEST_CASE("[BytecodeInterpreter] return : leaves the function right away, from any statement") {
	const auto result = Run(R"script(
		early = function(answer: int): string do
			if answer == 1
				return "right"
			end
			return "left " + answer
		end
		branches = function(answer: int): string do
			if answer == 1
				return "one"
			else
				return "other"
			end
		end
		inLoop = function(values: int[]): int do
			found = 0 - 1
			values | (value, index) do
				if value > 10
					return index
				end
			end
			return found
		end
		out = early(1) + " " + early(0) + " " + branches(1) + " " + branches(2) + " " + inLoop([1, 5, 12, 30]) + " " + inLoop([1])
	)script");
	CHECK(*result.nodeval<ska::StringShared>() == "right left 0 one other 2 -1");
}

TEST_CASE("[BytecodeInterpreter] return : without value, in a function without returned value") {
	const auto result = Run(R"script(
		count = 0
		increment = function(skip: bool) do
			if skip
				return
			end
			count = count + 1
		end
		increment(false)
		increment(true)
		increment(false)
		out = count
	)script");
	CHECK(result.nodeval<long>() == 2);
}

TEST_CASE("[BytecodeInterpreter] return : without value, in a function with a returned value, is rejected") {
	constexpr auto progStr = R"script(
		f = function(skip: bool): int do
			if skip
				return
			end
			return 1
		end
	)script";
	CHECK_THROWS(Interpret(progStr));
}

TEST_CASE("[BytecodeInterpreter] comments are ignored") {
	const auto result = Run(R"script(
		# Doubles a value
		double = function(v: int): int do # the parameter is an int
			# a comment in a function
			return v * 2 # returned
		end
		#
		out = double(21) # 42
	)script");
	CHECK(result.nodeval<long>() == 42);
}

TEST_CASE("[BytecodeInterpreter] a function parameter hides a script variable with the same name (it does not assign it)") {
	const auto result = Run(R"script(
		counter = 5
		entity = function(name: string): int do
			return 7
		end
		setCounter = function(counter: int) do
			counter = counter + 1
		end
		use = function(entity: int): int do
			return entity * 2
		end
		setCounter(100)
		used = use(entity("a"))
		out = counter * 100 + used + entity("b")
	)script");
	CHECK(result.nodeval<long>() == 5 * 100 + 14 + 7);
}
