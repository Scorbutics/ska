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
