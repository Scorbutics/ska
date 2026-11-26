#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"
#include "NodeValue/ScriptHandleAST.h"

TEST_CASE("[BytecodeGenerator] no type conversion string + string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = \"3\" + \"7\"\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_STR, "R0", "3", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float + float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3.0 + 7.4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_D, "R0", "3.000000", "7.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = [3]\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "3"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "1"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array + array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = [3] + [7, 12, 25]\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "3"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "1"},
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R1", "3"},
		{ska::bytecode::Command::PUSH_ARR_ARR, "R2", "R0", "R1"},
		{ska::bytecode::Command::MOV, "V0", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int - int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3 - 4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float - float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3.1 - 4.2\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB_D, "R0", "3.100000", "4.200000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array - array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = [11, 4, 9] - [1, 2]\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "11", "4", "9"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH, "1", "2"},
		{ska::bytecode::Command::POP_IN_ARR, "R1", "2"},
		{ska::bytecode::Command::SUB_ARR, "R2", "R0", "R1"},
		{ska::bytecode::Command::MOV, "V0", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int * int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3 * 4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MUL_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float * float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3.0 * 4.4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MUL_D, "R0", "3.000000", "4.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int / int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3 / 4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::DIV_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float / float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = 3.0 / 4.4\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::DIV_D, "R0", "3.000000", "4.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array explicit declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = []:int\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::POP_IN_ARR, "R0", "0"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array field size access") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("result = []:int\n test = result.size()\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::POP_IN_ARR, "R0", "0"},
		{ska::bytecode::Command::MOV, "V0", "R0"},
		{ska::bytecode::Command::ARR_LENGTH, "R1", "V0"},
		{ska::bytecode::Command::MOV, "V1", "R1"}
	});
}
