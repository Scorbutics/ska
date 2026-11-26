#pragma once
#include <string>
#include <unordered_map>

namespace ska {
	// Hints added to the compilation errors caused by the habits of other languages : how to write it in skalang
	namespace hints {

		inline std::string Format(const std::string& hint) {
			return hint.empty() ? "" : " (" + hint + ")";
		}

		// A name that is not a variable of the script
		inline std::string UnknownSymbol(const std::string& name) {
			static const auto hints = std::unordered_map<std::string, std::string> {
				{ "null", "there is no null value : a variable always has a value" },
				{ "nil", "there is no null value : a variable always has a value" },
				{ "None", "there is no null value : a variable always has a value" },
				{ "undefined", "there is no null value : a variable always has a value" },
				{ "let", "a variable is declared by assigning it : \"x = 1\"" },
				{ "const", "a variable is declared by assigning it : \"x = 1\"" },
				{ "def", "a function is a variable : \"f = function(a: int): int do ... end\"" },
				{ "fn", "a function is a variable : \"f = function(a: int): int do ... end\"" },
				{ "func", "a function is a variable : \"f = function(a: int): int do ... end\"" },
				{ "elif", "write \"else if\"" },
				{ "elsif", "write \"else if\"" },
				{ "then", "\"if condition\" (optional \"do\"), then the statements, then \"end\"" },
				{ "True", "the booleans are \"true\" and \"false\"" },
				{ "False", "the booleans are \"true\" and \"false\"" },
				{ "TRUE", "the booleans are \"true\" and \"false\"" },
				{ "FALSE", "the booleans are \"true\" and \"false\"" },
				{ "and", "the logical operators are \"&&\", \"||\" and \"!\"" },
				{ "or", "the logical operators are \"&&\", \"||\" and \"!\"" },
				{ "not", "the logical operators are \"&&\", \"||\" and \"!\"" },
				{ "this", "an object is built by a factory function returning { ... } : its functions use the variables of the factory" },
				{ "self", "an object is built by a factory function returning { ... } : its functions use the variables of the factory" },
				{ "new", "an object is built by calling a factory function (a function returning { ... })" },
				{ "print", "use the log module : Log = import \"bind:std.native.io.log\", then Log.print(text)" },
				{ "len", "use \".size()\" on an array or a string" },
				{ "break", "there is no \"break\" : use a condition in the loop, or \"return\" in a function" },
				{ "continue", "there is no \"continue\" : use an \"if\" in the loop" }
			};
			const auto hint = hints.find(name);
			return hint == hints.end() ? "" : Format(hint->second);
		}

		// A symbol that is not an operator of skalang
		inline std::string UnknownOperator(const std::string& op) {
			static const auto hints = std::unordered_map<std::string, std::string> {
				{ ";", "statements are separated by new lines, as the clauses of a for loop : \";\" is not used" },
				{ "'", "strings are between double quotes" },
				{ "++", "there is no increment operator : write \"i = i + 1\"" },
				{ "--", "there is no decrement operator : write \"i = i - 1\"" },
				{ "+=", "there is no compound assignment : write \"x = x + 1\"" },
				{ "-=", "there is no compound assignment : write \"x = x - 1\"" },
				{ "*=", "there is no compound assignment : write \"x = x * 2\"" },
				{ "/=", "there is no compound assignment : write \"x = x / 2\"" },
				{ "&", "the logical \"and\" is \"&&\"" },
				{ "?", "there is no ternary operator : use an \"if\"" }
			};
			const auto hint = hints.find(op);
			return hint == hints.end() ? "" : Format(hint->second);
		}

		// An operation between two different types
		inline std::string DifferentTypes() {
			return Format("no implicit conversion : convert explicitly with \":\", ex : (value: string), (count: float)");
		}

		inline std::string VariableKeyword() {
			return "a variable is declared by assigning it : \"x = 1\" (\"var\" is the type of the objects)";
		}

		inline std::string BraceBlock() {
			return Format("blocks are \"do ... end\" : { } build the object returned by a factory function");
		}

		inline std::string NamedFunction() {
			return "a function is a variable : write \"name = function(a: int): int do ... end\"";
		}
	}
}
