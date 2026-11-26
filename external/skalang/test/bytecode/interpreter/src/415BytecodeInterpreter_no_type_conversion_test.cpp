#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] no type conversion string + string") {
	static constexpr auto progStr = "result = \"3\" + \"7\"\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "37");
}

TEST_CASE("[BytecodeInterpreter] no type conversion float + float") {
	static constexpr auto progStr = "result = 3.0 + 7.4\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	CHECK(firstCellValue == 10.4);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array") {
	static constexpr auto progStr = "result = [3]\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
	CHECK((*firstCellValue)[0].nodeval<long>() == 3L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array + array") {
	static constexpr auto progStr = "result = [3] + [7, 12, 25]\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 4);
  CHECK((*firstCellValue)[0].nodeval<long>() == 3L);
  CHECK((*firstCellValue)[1].nodeval<long>() == 7L);
  CHECK((*firstCellValue)[2].nodeval<long>() == 12L);
  CHECK((*firstCellValue)[3].nodeval<long>() == 25L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int - int") {
	static constexpr auto progStr = "result = 3 - 4\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == -1);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float - float") {
	static constexpr auto progStr = "result = 3.1 - 4.2\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	CHECK(firstCellValue == -1.1);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array - array") {
	static constexpr auto progStr = "result = [11, 4, 9] - [1, 2]\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 1);
  CHECK((*firstCellValue)[0].nodeval<long>() == 11L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int * int") {
	static constexpr auto progStr = "result = 3 * 4\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 12);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float * float") {
	static constexpr auto progStr = "result = 3.0 * 4.4\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
  const auto check = firstCellValue + 0.01 >= 13.2 && firstCellValue - 0.01 <= 13.2;
	CHECK(check);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int / int") {
	static constexpr auto progStr = "result = 3 / 4\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 0);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float / float") {
	static constexpr auto progStr = "result = 3.0 / 4.4\n";
  auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
  const auto check = firstCellValue + 0.000001 >= 0.68181818181 && firstCellValue - 0.000001 <= 0.68181818181;
	CHECK(check);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array explicit declaration") {
	static constexpr auto progStr = "result = []:int\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto value = res.as<ska::NodeValueArray>();
	const auto check = value != nullptr && value->empty();
	CHECK(check);
}
