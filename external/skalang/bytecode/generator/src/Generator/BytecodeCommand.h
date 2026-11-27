#pragma once
#include <unordered_map>
#include <ostream>

namespace ska {
	namespace bytecode {
		enum class Command {
			MOV,
			CONV_I_STR,
			CONV_STR_I,
			CONV_D_STR,
			CONV_STR_D,
			CONV_I_D,
			CONV_D_I,
			PUSH_F_ARR,
			PUSH_ARR_ARR,
			PUSH_B_ARR,
			POP_F_ARR,
			POP_B_ARR,
			POP_IN_ARR,
			POP_IN_VAR,
			ARR_ACCESS,
			ARR_LENGTH,
			// ARR_SET object index value : stores a value in an array cell / object field (writes into the field cell if it is a captured variable)
			ARR_SET,
			JUMP_REL,
			JUMP_ABS,
			BIND,
			JUMP_NIF,
			PUSH,
			POP,
			FIELD,
			SCRIPT,
			RET,
			END,
			MUL_I,
			MUL_I_STR,
			MUL_STR_I,
			MUL_I_ARR,
			MUL_ARR_I,
			MUL_D,
			ADD_I,
			ADD_STR,
			ADD_D,
			DIV_I,
			DIV_D,
			SUB_I,
			SUB_D,
			SUB_ARR,
			CMP_STR,
			CMP_ARR,
			TEST_EQ,
			TEST_NEQ,
			TEST_G,
			TEST_GE,
			TEST_L,
			TEST_LE,
			// CELL local : makes a local variable slot a cell (the variable is captured by a closure)
			CELL,
			// PUSH_CELL local|upval : pushes the cell of a captured variable (to build a closure, or an object field aliasing it)
			PUSH_CELL,
			// CMP_BOOL dest left right : 0 if both booleans are equal, 1 otherwise (followed by TEST_EQ / TEST_NEQ)
			CMP_BOOL,
			// MOD_I dest left right : remainder of the integer division
			MOD_I,
			// STR_LENGTH dest string : number of characters of a string
			STR_LENGTH,
			// STR_ACCESS dest string index : character of a string (a string)
			STR_ACCESS,
			// CONV_B_STR dest bool : "true" or "false"
			CONV_B_STR,
			// MAP_NEW dest : a new empty map
			MAP_NEW,
			// MAP_ACCESS dest map key : value of the key (error when the map has no such key)
			MAP_ACCESS,
			// MAP_SET map key value : stores the value under the key
			MAP_SET,
			// MAP_HAS dest map key : true when the map has the key
			MAP_HAS,
			// MAP_KEYS dest map : the keys (string array, sorted)
			MAP_KEYS,
			// MAP_SIZE dest map : number of keys
			MAP_SIZE,
			// ARR_HAS dest array value : true when an element of the array is equal to the value
			ARR_HAS,
			// ENUM_CHECK dest string names : the string, when it is one of the names (string array) of an enum, an error otherwise
			ENUM_CHECK,
			NOP,
			UNUSED_Last_Length
		};

		static constexpr const char* CommandSTR[] = {
			"MOV",
			"CONV_I_STR",
			"CONV_STR_I",
			"CONV_D_STR",
			"CONV_STR_D",
			"CONV_I_D",
			"CONV_D_I",
			"PUSH_F_ARR",
			"PUSH_ARR_ARR",
			"PUSH_B_ARR",
			"POP_F_ARR",
			"POP_B_ARR",
			"POP_IN_ARR",
			"POP_IN_VAR",
			"ARR_ACCESS",
			"ARR_LENGTH",
			"ARR_SET",
			"JUMP_REL",
			"JUMP_ABS",
			"BIND",
			"JUMP_NIF",
			"PUSH",
			"POP",
			"FIELD",
			"SCRIPT",
			"RET",
			"END",
			"MUL_I",
			"MUL_I_STR",
			"MUL_STR_I",
			"MUL_I_ARR",
			"MUL_ARR_I",
			"MUL_D",
			"ADD_I",
			"ADD_STR",
			"ADD_D",
			"DIV_I",
			"DIV_D",
			"SUB_I",
			"SUB_D",
			"SUB_ARR",
			"CMP_STR",
			"CMP_ARR",
			"TEST_EQ",
			"TEST_NEQ",
			"TEST_G",
			"TEST_GE",
			"TEST_L",
			"TEST_LE",
			"CELL",
			"PUSH_CELL",
			"CMP_BOOL",
			"MOD_I",
			"STR_LENGTH",
			"STR_ACCESS",
			"CONV_B_STR",
			"MAP_NEW",
			"MAP_ACCESS",
			"MAP_SET",
			"MAP_HAS",
			"MAP_KEYS",
			"MAP_SIZE",
			"ARR_HAS",
			"ENUM_CHECK",
			"NOP",
			""
		};

		static_assert(sizeof(CommandSTR) / sizeof(CommandSTR[0]) == (static_cast<std::size_t>(Command::UNUSED_Last_Length) + 1));

		static inline std::ostream& operator<<(std::ostream& stream, const Command& cmd) {
			stream << CommandSTR[static_cast<std::size_t>(cmd)];
			return stream;
		}

	}
}