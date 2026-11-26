#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] empty if") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true )\n end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "0", "1" }
	});
}

TEST_CASE("[BytecodeGenerator] if with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) \n toto = 5\n end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "1", "1" },
		{ Command::MOV, "V0", "5" }
	});
}

TEST_CASE("[BytecodeGenerator] if else with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( 3 > 2 )\n toto = 1 + 3\n else\n toto = 4 + 2 + 1\n tt = toto\n end tete = 444\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::SUB_I, "R0", "3", "2" },
		{ Command::TEST_G, "R0", "R0" },
		{ Command::JUMP_NIF, "3", "R0" },
		{ Command::ADD_I, "R1", "1", "3" },
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "4" },
		{ Command::ADD_I, "R2", "4", "2" },
		{ Command::ADD_I, "R3", "R2", "1" },
		{ Command::MOV, "V1", "R3" },
		{ Command::MOV, "V2", "V1" },
		{ Command::MOV, "V3", "444" }
	});

}

TEST_CASE("[BytecodeGenerator] for with empty body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(i = 0\n i < 10\n i = i + 1) do end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },

		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "3", "R0" },

		// Body part

		// Increment part
		{ Command::ADD_I, "R1", "V0", "1"},
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "-6" }
	});
}

TEST_CASE("[BytecodeGenerator] for with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(i = 0\n i < 10\n i = i + 1) do toto = 123\n toto + i\n end test = 1234\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },

		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "5", "R0" },

		// Body part
		{ Command::MOV, "V1", "123" },
		{ Command::ADD_I, "R1", "V1", "V0" },

		// Increment part
		{ Command::ADD_I, "R2", "V0", "1"},
		{ Command::MOV, "V0", "R2" },
		{ Command::JUMP_REL, "-8" },

		// Post part
		{ Command::MOV, "V2", "1234"}
	});
}

TEST_CASE("[BytecodeGenerator] array filter without index but with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("array97 = [0, 2, 3]\n array97 | (iteratorArray97) do \n iteratorArray97 = iteratorArray97 + 1\n end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		//	Array
		{ Command::PUSH, "0", "2", "3" },
		{ Command::POP_IN_ARR, "R0", "3" },
		{ Command::MOV, "V0", "R0" },

		//	Iterator & array length
		{ Command::ARR_LENGTH, "R1", "V0" },
		{ Command::MOV, "R2", "0"},

		// Loop init
		{ Command::SUB_I, "R3", "R2", "R1" },
		{ Command::TEST_L, "R3", "R3" },
		{ Command::JUMP_NIF, "5", "R3" },

		//	Pre-body (element access)
		{ Command::ARR_ACCESS, "V1", "V0", "R2"},

		// Body part
		{ Command::ADD_I, "R4", "V1", "1" },
		{ Command::MOV, "V1", "R4" },

		// Increment part
		{ Command::ADD_I, "R2", "R2", "1"},
		{ Command::JUMP_REL, "-8" },

	});
}

TEST_CASE("[BytecodeGenerator] array filter with index with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("array97 = [0, 2, 3]\n array97 | (iteratorArray97, index) do \n iteratorArray97 = iteratorArray97 + index\n end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		//	Array
		{ Command::PUSH, "0", "2", "3" },
		{ Command::POP_IN_ARR, "R0", "3" },
		{ Command::MOV, "V0", "R0" },

		//	Iterator, index & array length
		{ Command::ARR_LENGTH, "R1", "V0" },
		{ Command::MOV, "V1", "0"},

		// Loop init
		{ Command::SUB_I, "R2", "V1", "R1" },
		{ Command::TEST_L, "R2", "R2" },
		{ Command::JUMP_NIF, "5", "R2" },

		//	Pre-body (element access)
		{ Command::ARR_ACCESS, "V2", "V0", "V1"},

		// Body part
		{ Command::ADD_I, "R3", "V2", "V1" },
		{ Command::MOV, "V2", "R3" },

		// Increment part
		{ Command::ADD_I, "V1", "V1", "1"},
		{ Command::JUMP_REL, "-8" },

		});
}
