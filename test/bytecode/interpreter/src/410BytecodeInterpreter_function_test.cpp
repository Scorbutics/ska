#include <doctest.h>
#include <tuple>
#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] function with 4 parameters (> 3) + use") {
	auto [script, data] = Interpret(
		"test1 = 0-1\n"
		"test2 = 0-1\n"
		"test3 = 0-1\n"
		"test4 = 0-1\n"
		"toto = function(t: int, t1: int, t2: int, t3: int) do\n"
			"test1 = t\n"
			"test2 = t1\n"
			"test3 = t2\n"
			"test4 = t3\n"
		"end\n"
		"toto(1, 2, 3, 4)\n"
		"final = test1: string + test2: string + test3: string + test4: string\n");

	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "1234");
}

TEST_CASE("[BytecodeInterpreter] Custom object creation (field access)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"t = test.test\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 123);
}

TEST_CASE("[BytecodeInterpreter] Custom object creation 2 (field function call)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n"
		"t = test.say(\"titi4\")\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));

	//ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());

	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "lol123titi4");
}

// We have to check that calling a function several times with different parameters
// still refers to the same function and the resulting value is different
TEST_CASE("[BytecodeInterpreter] Custom object creation 3 (double field function call)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n"
		"t = test.say(\"titi4\")\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "lol123titi4");
}

TEST_CASE("[BytecodeInterpreter] using a function as a parameter") {
	constexpr auto progStr =
		"bi_193 = function() : var do\n"
		"return { test = 14 }\n"
		"end\n"
		"bi_209 = function(toto: bi_193) : bi_193() do\n"
		"return toto()\n"
		"end\n"
		"object = bi_209(bi_193)\n"
		"t = object.test\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 14);
}

TEST_CASE("[BytecodeInterpreter] down scope function variable access") {
	constexpr auto progStr =
		"testValue = 0\n"
		"callback = function() do testValue = 1\n end\n"
		"callback()\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 1);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter") {
	constexpr auto progStr =
		"testValue = 1234\n"
		"callback = function() do testValue = 789\n end\n"
		"lvalFunc219 = function(toto: callback) do\n"
		" toto()\n"
		"end\n"
		"lvalFunc219(callback)\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter without using the source type (function type compatibility)") {
	constexpr auto progStr =
		"lvalFunc218 = function() do end\n"
		"lvalFunc219 = function(toto: lvalFunc218) : lvalFunc218() do\n"
		" toto()\n"
		"end\n"
		"testValue = 1234\n"
		"callback = function() do testValue = 789\n end\n"
		"lvalFunc219(callback)\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}

TEST_CASE("[BytecodeInterpreter] closure: everything inside factory function is relative to object instance") {
	constexpr auto progStr =
		"TestFcty = function(value_: string): var do\n"
		"return {\n"
		"value = function(): string do\n"
		"return value_\n"
		"end\n"
		"}\n"
		"end\n"

		"toto = TestFcty(\"toto\")\n"
		"titi = TestFcty(\"titi\")\n"
		"out = toto.value() + titi.value()\n";

	// We expect toto.value() == "toto" and titi.value() == "titi"
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());
#endif
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "tototiti");
}


TEST_CASE("[BytecodeInterpreter] closure: can modify stored value in captured env.") {
	constexpr auto progStr = R"script(
		TestFcty = function(value_: string): var do
			internal = value_
			return {
				value = function(): string do
					return internal
				end
				setValue = function(v: string) do
					internal = v
				end
			}
		end

		toto = TestFcty("toto")
		toto.setValue("titi")
		out = toto.value()
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());
#endif
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "titi");
}

TEST_CASE("[BytecodeInterpreter] can modify stored values in direct access") {
	constexpr auto progStr = R"script(
		TestFcty = function(value_: string): var do
			return {
				value = value_
			}
		end

		toto = TestFcty("toto")
		toto.value = "titi"
		out = toto.value + ""
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());
#endif
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "titi");
}


TEST_CASE("[BytecodeInterpreter] can modify stored values in direct access and check using getter") {
	constexpr auto progStr = R"script(
		TestFcty = function(value_: string): var do
			return {
				value = value_
				get = function(): string do
					return value
				end
			}
		end

		toto = TestFcty("toto")
		toto.value = "titi"
		out = toto.get()
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());
#endif
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "titi");
}

TEST_CASE("[BytecodeInterpreter] object field named like a factory local is a distinct field, and reading a field copies its value") {
	constexpr auto progStr = R"script(
		Fcty = function(): var do
			test = 3
			return {
				test = test
				tata = "44"
			}
		end

		titi = Fcty()
		before = titi.test
		titi.test = 5
		out = "" + before + "/" + titi.test + "/" + titi.tata
	)script";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	// "before" keeps the value read before the field modification, and "tata" is still accessible at its own index
	CHECK(*firstCellValue == "3/5/44");
}
