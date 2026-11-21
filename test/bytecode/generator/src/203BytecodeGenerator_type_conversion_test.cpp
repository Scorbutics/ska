#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

TEST_CASE("[BytecodeGenerator] type conversion (explicit) float => int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 7.0: int\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_D_I, "R0", "7.000000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion (explicit) int => float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 7: float\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_D, "R0", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion (explicit) int => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 7: string\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_STR, "R0", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
		});
}

TEST_CASE("[BytecodeGenerator] type conversion (explicit) float => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 7.0: string\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_D_STR, "R0", "7.000000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion (explicit) string => float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = \"7\": float\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_STR_D, "R0", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion (explicit) string => int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = \"7\": int\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_STR_I, "R0", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] explicit conversion int => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(" result = (7: string) + \"3\"\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_STR, "R0", "7"},
		{ska::bytecode::Command::ADD_STR, "R1", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] explicit conversion int => float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(" result = 7.0 + (3: float)\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_D, "R0", "3"},
		{ska::bytecode::Command::ADD_D, "R1", "7.000000", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (back)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(" result = [7, 12, 25] + 3\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH_B_ARR, "R1", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (front)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(" result = 3 + [7, 12, 25]\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH_F_ARR, "R1", "3", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}
