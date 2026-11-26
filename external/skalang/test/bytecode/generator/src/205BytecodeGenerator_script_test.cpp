#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"
#include "NodeValue/ScriptHandleAST.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] import") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("Player = import \"" SKALANG_TEST_DIR "/src/resources/play\"\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{ Command::SCRIPT, "R0", "1" },
		{ Command::MOV, "V0", "R0" }
	});
}

TEST_CASE("[BytecodeGenerator] Outside script from file (import) and use") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(
		"Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"player = Character184.build(\"Player\")\n"
		"enemy = Character184.default\n enemy.age\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	//CHECK(res.nodeval<int>() == 10);
}

TEST_CASE("[BytecodeGenerator] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"Character91 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character92 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character91.default.age = 123\n"
		"t = Character92.default.age\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(gen, {
		{ Command::SCRIPT, "R0", "1" },
		{ Command::MOV, "V0", "R0" },
		{ Command::SCRIPT, "R1", "1" },
		{ Command::MOV, "V1", "R1" },
		{ Command::ARR_ACCESS, "R2", "V0", "2" },
		{ Command::ARR_SET, "R2", "0", "123" },
		{ Command::ARR_ACCESS, "R3", "V1", "2" },
		{ Command::ARR_ACCESS, "R4", "R3", "0" },
		{ Command::MOV, "V2", "R4" },
	});
}

TEST_CASE("[BytecodeGenerator] Outside script from file (import) - edit - and use") {
	constexpr auto progStr =
		"Character260 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"enemy = Character260.default\n"
		"enemy.age = 99\n"
		"t = enemy.age\n";
	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	BytecodeCompare(data.storage->at(1), {
		{ Command::JUMP_REL, "8" },
		{ Command::POP, "L0" },
		{ Command::MOV, "L1", "10" },
		{ Command::MOV, "L2", "L1" },
		{ Command::PUSH, "L2" },
		{ Command::MOV, "L3", "L0" },
		{ Command::PUSH, "L3" },
		{ Command::POP_IN_VAR, "R0", "2" },
		{ Command::RET, "R0" },
		{ Command::END, "V0", "-9" },
		{ Command::MOV, "V1", "Default" },
		{ Command::PUSH, "V1" },
		{ Command::JUMP_ABS, "V0" },
		{ Command::POP, "R0" },
		{ Command::MOV, "V2", "R0" },
	});
	BytecodeCompare(gen, {
		{ Command::SCRIPT, "R0", "1" },
		{ Command::MOV, "V0", "R0" },
		{ Command::ARR_ACCESS, "R1", "V0", "2" },
		{ Command::MOV, "V1", "R1" },
		{ Command::ARR_SET, "V1", "0", "99" },
		{ Command::ARR_ACCESS, "R2", "V1", "0" },
		{ Command::MOV, "V2", "R2" },
	});
}

TEST_CASE("[BytecodeGenerator] C++ 1 script-function binding") {
	/*auto [astPtr, data] = ASTFromInputBytecodeGenerator("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto& res = data.generator->generate(std::move(astPtr));

	BytecodeCompare(res, {
	});*/

	/*
	auto data = ASTFromInputBytecodeGeneratorNoParse("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto test = 0;
	auto testStr = std::string{ "" };
	auto function = std::function<int(ska::Script&, int, ska::StringShared)>(
		[&](ska::Script&, int toto, ska::StringShared titi) -> int {
		test = toto;
		testStr = std::move(*titi);
		return 0;
	});


	auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
	scriptBinding.bindFunction("funcTest", std::move(function));
	scriptBinding.buildFunctions();

	readerI->parse(*data.parser);
	auto generated = data.generator->generate(ska::bytecode::Script{ *readerI });
	*/
}

