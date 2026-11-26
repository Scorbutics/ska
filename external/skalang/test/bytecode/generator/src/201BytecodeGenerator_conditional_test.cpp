#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4 == 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] var declaration : conditional") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = 4 == 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"},
		{Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] conditional strings") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("\"4\" == \"1\"\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::CMP_STR, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] conditional arrays") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("[4] == [1]\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::PUSH, "4"},
		{Command::POP_IN_ARR, "R0", "1"},
		{Command::PUSH, "1"},
		{Command::POP_IN_ARR, "R1", "1"},
		{Command::CMP_ARR, "R2", "R0", "R1"},
		{Command::TEST_EQ, "R2", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] different") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 != 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_NEQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] greater than") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 > 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_G, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] greater than or equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 >= 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_GE, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] lesser than") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 < 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_L, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] lesser than or equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 <= 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_LE, "R0", "R0"}
	});
}
