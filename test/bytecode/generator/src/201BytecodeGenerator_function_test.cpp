#include <iostream>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"
#include "NodeValue/ScriptHandleAST.h"

TEST_CASE("[BytecodeGenerator] Empty function only void") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function() do end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-2" }
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 1 parameter") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(t: int) do end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-3"},
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	static constexpr auto progStr = "toto = function(t: int, t1: string, t2: int, t3: int) do end\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "L0", "L1", "L2"},
		{ska::bytecode::Command::POP, "L3"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-4"},
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3) + use") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(t: int, t1: int, t2: int, t3: int) do test=t\ntest=t1\ntest=t2\ntest=t3\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "7"},
		{ska::bytecode::Command::POP, "L0", "L1", "L2"},
		{ska::bytecode::Command::POP, "L3"},
		{ska::bytecode::Command::MOV, "L4", "L0"},
		{ska::bytecode::Command::MOV, "L4", "L1"},
		{ska::bytecode::Command::MOV, "L4", "L2"},
		{ska::bytecode::Command::MOV, "L4", "L3"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-8"},
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::MOV, "R0", "0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-3"},
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::MOV, "R0", "0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-4"},
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do result = test + 3\n return result\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "5"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::ADD_I, "R0", "L0", "3"},
		{ska::bytecode::Command::MOV, "L1", "R0"},
		{ska::bytecode::Command::MOV, "R1", "L1"},
		{ska::bytecode::Command::RET, "R1"},
		{ska::bytecode::Command::END, "V0", "-6"},
	});
}


TEST_CASE("[BytecodeGenerator] Custom object creation") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 1\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "17"},
		{ska::bytecode::Command::CELL, "L0"},
		{ska::bytecode::Command::MOV, "L0", "1"},
		{ska::bytecode::Command::MOV, "L1", "L0"},
		{ska::bytecode::Command::PUSH, "L1"},
		{ska::bytecode::Command::JUMP_REL, "7"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "U0"},
		{ska::bytecode::Command::ADD_STR, "R0", "lol", "R0"},
		{ska::bytecode::Command::ADD_STR, "R1", "R0", "L0"},
		{ska::bytecode::Command::MOV, "L1", "R1"},
		{ska::bytecode::Command::MOV, "R2", "L1"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::PUSH_CELL, "L0"},
		{ska::bytecode::Command::END, "L2", "-9", "1"},
		{ska::bytecode::Command::PUSH, "L2"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "2"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-18"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R0"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
	});
}

TEST_CASE("[BytecodeGenerator] Use complex call inside function parameters") {
	constexpr auto progStr =
		"titi = function() : var do return { test = \"tulululu\" }\n end\n"
		"toto = function(arg1 : string) : void do end\n"
		"toto(titi().test)\n";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::MOV, "L0", "tulululu"},
		{ska::bytecode::Command::PUSH, "L0"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-5"},
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V1", "-3"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R0"},
		{ska::bytecode::Command::ARR_ACCESS, "R1", "R0", "0"},
		{ska::bytecode::Command::PUSH, "R1"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
	});
}

TEST_CASE("[BytecodeGenerator] Custom object creation2") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(script));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "17"},
		{ska::bytecode::Command::CELL, "L0"},
		{ska::bytecode::Command::MOV, "L0", "123"},
		{ska::bytecode::Command::MOV, "L1", "L0"},
		{ska::bytecode::Command::PUSH, "L1"},
		{ska::bytecode::Command::JUMP_REL, "7"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "U0"},
		{ska::bytecode::Command::ADD_STR, "R0", "lol", "R0"},
		{ska::bytecode::Command::ADD_STR, "R1", "R0", "L0"},
		{ska::bytecode::Command::MOV, "L1", "R1"},
		{ska::bytecode::Command::MOV, "R2", "L1"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::PUSH_CELL, "L0"},
		{ska::bytecode::Command::END, "L2", "-9", "1"},
		{ska::bytecode::Command::PUSH, "L2"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "2"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-18"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R0"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::ARR_ACCESS, "R1", "V1", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "R1"},
		{ska::bytecode::Command::POP, "R2"},
	});
}

// We have to check that calling a function several times stills refer to the same
// JUMP_ABS instruction
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
		"test.say(\"titi4\")\n";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(script));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "17"},
		{ska::bytecode::Command::CELL, "L0"},
		{ska::bytecode::Command::MOV, "L0", "123"},
		{ska::bytecode::Command::MOV, "L1", "L0"},
		{ska::bytecode::Command::PUSH, "L1"},
		{ska::bytecode::Command::JUMP_REL, "7"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "U0"},
		{ska::bytecode::Command::ADD_STR, "R0", "lol", "R0"},
		{ska::bytecode::Command::ADD_STR, "R1", "R0", "L0"},
		{ska::bytecode::Command::MOV, "L1", "R1"},
		{ska::bytecode::Command::MOV, "R2", "L1"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::PUSH_CELL, "L0"},
		{ska::bytecode::Command::END, "L2", "-9", "1"},
		{ska::bytecode::Command::PUSH, "L2"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "2"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-18"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R0"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::ARR_ACCESS, "R1", "V1", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "R1"},
		{ska::bytecode::Command::POP, "R2"},
		{ska::bytecode::Command::ARR_ACCESS, "R3", "V1", "1"},
		{ska::bytecode::Command::PUSH, "titi4"},
		{ska::bytecode::Command::JUMP_ABS, "R3"},
		{ska::bytecode::Command::POP, "R4"},
	});
}

TEST_CASE("[BytecodeGenerator] Field access affectation") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end"
			"}\n"
		"end\n"
		"t = toto()\n"
		"t.test = 1122\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(astPtr));
}

TEST_CASE("[BytecodeGenerator] everything inside factory function is relative to object instance") {
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

	"toutou = toto.value\n"
	"if true\n"
	"toutou = titi.value\n"
	"end\n"
	"toutou()\n";

	// We expect toto.value() == "toto" and titi.value() == "titi"
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, res.id());
#endif

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "10"},
		{ska::bytecode::Command::CELL, "L0"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::MOV, "R0", "U0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::PUSH_CELL, "L0"},
		{ska::bytecode::Command::END, "L1", "-4", "1"},
		{ska::bytecode::Command::PUSH, "L1"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-11"},
		{ska::bytecode::Command::PUSH, "toto"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R0"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::MOV, "V2", "R1"},
		{ska::bytecode::Command::ARR_ACCESS, "R2", "V1", "0"},
		{ska::bytecode::Command::MOV, "V3", "R2"},
		{ska::bytecode::Command::JUMP_NIF, "2", "1"},
		{ska::bytecode::Command::ARR_ACCESS, "R3", "V2", "0"},
		{ska::bytecode::Command::MOV, "V3", "R3"},
		{ska::bytecode::Command::JUMP_ABS, "V3"},
		{ska::bytecode::Command::POP, "R4"},
	});
}

TEST_CASE("[BytecodeGenerator] callback array") {
	constexpr auto progStr =
		"Default = function() do end\n"

		"call = function(callback: Default[]) do\n"
			"callback[0]()\n"
		"end\n"

		"call([Default])\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1" },
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V0", "-2" },
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::POP, "L0"},
		{ska::bytecode::Command::ARR_ACCESS, "R0", "L0", "0"},
		{ska::bytecode::Command::JUMP_ABS, "R0" },
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V1", "-5"},
		{ska::bytecode::Command::PUSH, "V0" },
		{ska::bytecode::Command::POP_IN_ARR, "R0", "1"},
		{ska::bytecode::Command::PUSH, "R0"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
	});
}
