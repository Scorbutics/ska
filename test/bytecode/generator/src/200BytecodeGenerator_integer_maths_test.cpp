#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

TEST_CASE("[BytecodeGenerator] literal alone") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, { });
}

TEST_CASE("[BytecodeGenerator] var declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = 4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"}
	});
}

TEST_CASE("[BytecodeGenerator] var declaration from var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = 4\n titi = toto\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"},
		{ska::bytecode::Command::MOV, "V1", "V0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths linear") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 + 4 - 1\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::SUB_I, "R1", "R0", "1"},
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 left subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * 2\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::MUL_I, "R1", "R0", "2"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 right subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("2 * (3 + 4)\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::MUL_I, "R1", "2", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths subparts") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * (1 + 2)\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::ADD_I, "R1", "1", "2"},
		{ska::bytecode::Command::MUL_I, "R2", "R0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths with var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = 4\n (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"},
		{ska::bytecode::Command::MUL_I, "R0", "V0", "5"},
		{ska::bytecode::Command::ADD_I, "R1", "3", "4"},
		{ska::bytecode::Command::DIV_I, "R2", "1", "4"},
		{ska::bytecode::Command::SUB_I, "R3", "R1", "R2"},
		{ska::bytecode::Command::MUL_I, "R4", "2", "R3"},
		{ska::bytecode::Command::ADD_I, "R5", "R0", "R4"},
		{ska::bytecode::Command::ADD_I, "R6", "R5", "1"},
		{ska::bytecode::Command::ADD_I, "R7", "R6", "9"}
	});
}

TEST_CASE("[BytecodeGenerator] var expression declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 7 + 3\n");
	auto& res = data.generator->generate(*data.storage, { std::move(astPtr) });

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "7", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}
