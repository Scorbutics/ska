#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] explicit type conversion string => float") {
	static constexpr auto progStr = "result = \"7.0\" :float\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	CHECK(firstCellValue == 7.0);
}

TEST_CASE("[BytecodeInterpreter] explicit type conversion string => int") {
	static constexpr auto progStr = "result = \"3\" :int\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 3);
}

TEST_CASE("[BytecodeInterpreter] explicit type conversion string => int + with computing after") {
	static constexpr auto progStr = "result = \"3\" :int * 5 + 2\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 17);
}

TEST_CASE("[BytecodeInterpreter] no implicit conversion : int + string, float + string, float + int are rejected") {
	CHECK_THROWS(Interpret("result = 7 + \"3\"\n"));
	CHECK_THROWS(Interpret("result = \"3\" + 7\n"));
	CHECK_THROWS(Interpret("result = 7.0 + \"3\"\n"));
	CHECK_THROWS(Interpret("result = 7.0 + 3\n"));
	CHECK_THROWS(Interpret("result = 3 < 7.0\n"));
	CHECK_THROWS(Interpret("result = 3\n result = \"3\"\n"));
}

TEST_CASE("[BytecodeInterpreter] explicit conversions") {
	static constexpr auto progStr = "result = (7: string) + \"3\" + \"/\" + (7.5: string) + \"/\" + ((7.0 + (3: float)): string)\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(*res.nodeval<ska::StringShared>() == "73/7.5/10");
}

TEST_CASE("[BytecodeInterpreter] type conversion + int => array (back)") {
	static constexpr auto progStr = "result = [7, 12, 25] + 3\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 4);
	CHECK((*firstCellValue)[0].nodeval<long>() == 7);
  CHECK((*firstCellValue)[1].nodeval<long>() == 12);
  CHECK((*firstCellValue)[2].nodeval<long>() == 25);
  CHECK((*firstCellValue)[3].nodeval<long>() == 3);
}

TEST_CASE("[BytecodeInterpreter] type conversion + int => array (front)") {
	static constexpr auto progStr = "result = 3 + [7, 12, 25]\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
	CHECK(firstCellValue->size() == 4);
	CHECK((*firstCellValue)[0].nodeval<long>() == 3);
  CHECK((*firstCellValue)[1].nodeval<long>() == 7);
  CHECK((*firstCellValue)[2].nodeval<long>() == 12);
  CHECK((*firstCellValue)[3].nodeval<long>() == 25);
}
