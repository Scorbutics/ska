#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] equal int (false)") {
	static constexpr auto progStr = "t= 4 == 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] equal int (true)") {
	static constexpr auto progStr = "t= 41 == 41\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] conditional strings") {
	static constexpr auto progStr = "t= \"4\" == \"1\"\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);

}

TEST_CASE("[BytecodeInterpreter] different") {
	static constexpr auto progStr = "t= 3 != 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] greater than") {
	static constexpr auto progStr = "t= 3 > 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] greater than or equal") {
	static constexpr auto progStr = "t= 3 >= 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] lesser than") {
	static constexpr auto progStr = "t= 3 < 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] lesser than or equal") {
	static constexpr auto progStr = "t= 3 <= 1\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] conditional arrays") {
	static constexpr auto progStr = "t= [4] == [1]\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}
