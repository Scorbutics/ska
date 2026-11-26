#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] if : if body") {
	static constexpr auto progStr =
	"t = false\n"
	"if ([18] == [18]) \n "
	"t = true "
	"end";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] if : else body") {
	static constexpr auto progStr =
	"t = 0\n"
	"if ([18] == [1124])\n"
	"t = 1\n"
	"else\n"
	"t = 2 "
	"end\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 2);
}

TEST_CASE("[BytecodeInterpreter] for without body") {
	static constexpr auto progStr = "for(i = 0\n i < 10\n i = i + 1) do end\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] for with body") {
	static constexpr auto progStr = "toto = 123\n for(i = 0\n i < 10\n i = i + 1) do toto = toto + i * 2\n end test = toto\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 213);
}

TEST_CASE("[BytecodeInterpreter] filter with body without iterator index") {
	static constexpr auto progStr = "tmp = 0\n [0, 2, 3] | (iteratorArray97) do \n tmp = tmp + iteratorArray97\n end test = tmp\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 5);
}

TEST_CASE("[BytecodeInterpreter] filter with body with iterator index") {
	static constexpr auto progStr = "tmp = 0\n [0, 2, 3] | (iteratorArray97, index) do \n tmp = tmp + iteratorArray97 + index\n end test = tmp\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 8);
}
