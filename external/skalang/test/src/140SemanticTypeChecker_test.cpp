#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptHandleAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

auto readerSTC = std::unique_ptr<ska::ScriptAST>{};

static ska::ScriptAST ASTFromInputSemanticTC(ska::ScriptCacheAST& scriptCache, const std::string& input, DataTestContainer& data) {

	auto tokenizer = ska::Tokenizer { data.reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto typeCrosser = ska::TypeCrosser{};
	readerSTC = std::make_unique<ska::ScriptAST>(scriptCache, "main", tokens );
	data.parser = std::make_unique<ska::StatementParser> ( data.reservedKeywords );
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosser);
	readerSTC->parse(*data.parser);
	return *readerSTC;
}

TEST_CASE("[SymbolTableUpdater] update node symbol") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

	auto astPtr = ASTFromInputSemanticTC(scriptCache, "i = \"0\"\n titi = \"llllll\"\n do toto = 2\n i = titi\n end", data);

	auto& table = astPtr.symbols();

	const auto* i = table["i"];
	const auto* nestedToto = table.lookup(ska::SymbolTableLookup::hierarchical("toto"), ska::SymbolTableNested::lastChild());
	const auto* toto = table["toto"];
	const auto* titi = table["titi"];
	const auto* nestedI = table.lookup(ska::SymbolTableLookup::hierarchical("i"), ska::SymbolTableNested::lastChild());

	auto& ast = astPtr.rootNode()[0];
	CHECK(i == ast[0].symbolTable());
	CHECK(titi == ast[1].symbolTable());
	CHECK(ast[1].symbol() != ast[0].symbol());
	CHECK(nestedToto == ast[2][0].symbolTable());
	CHECK(nestedI == ast[2][1].symbolTable());
	CHECK(nestedI == i);
}

TEST_CASE("[SymbolTableUpdater] function declaration") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

	auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = function() do end\n", data);

	auto& table = astPtr.symbols();

	const auto* toto = table["toto"];

	auto& ast = astPtr.rootNode()[0];
	CHECK(toto != nullptr);
	CHECK(toto == ast[0].symbolTable());
	CHECK(toto == ast[0][0].symbolTable());
}

TEST_CASE("[SemanticTypeChecker]") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

	SUBCASE("OK") {

		SUBCASE("function with invalid converter operator name") {

			try {
				static constexpr auto progStr = R"script(
				testFcty81 = function() :var do
					return {
						:intuiutututut do
							return "1"
						end
					}
				end
				)script";
				ASTFromInputSemanticTC(scriptCache, progStr, data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{ e.what() }.find("undeclared custom type \"intuiutututut\"") != std::string::npos);
			}
		}

		SUBCASE("function with incompatible return for a converter operator") {

				static constexpr auto progStr = R"script(
				testFcty99 = function() :var do
					return {
						:float do
							return "1"
						end
					}
				end
				)script";
				CHECK_THROWS_WITH(ASTFromInputSemanticTC(scriptCache, progStr, data), doctest::Contains("bad return type : expected \"float\" on function declaration but got \"string\""));
		}

		SUBCASE("unable to call a converter as a normal function call") {
			try {
				static constexpr auto progStr =
					"testFcty189 = function() :var do\n"
					"return {\n"
					":int do\n"
					"return 1\n"
					"end\n"
					"}\n"
					"end\n"
					"testFcty189().:int()\n";
				ASTFromInputSemanticTC(scriptCache, progStr, data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{ e.what() }.find("bad token matching : expected \"a token with type \"IDENTIFIER\"\" but got \":\"") != std::string::npos);
			}
		}

		SUBCASE("call a converter as type conversion call") {
			static constexpr auto progStr =
				"testFcty189 = function() :var do\n"
				"return {\n"
				":int do\n"
				"return 1\n"
				"end\n"
				"}\n"
				"end\n"
				"testFcty189()\n"
				"testFcty189(): int\n";
			auto astPtr = ASTFromInputSemanticTC(scriptCache, progStr, data);
			auto& ast = astPtr.rootNode()[0];
			CHECK(ast.size() == 3);
			CHECK(ast[1].type() == ska::ExpressionType::OBJECT);
			CHECK(ast[2].type() == ska::ExpressionType::INT);
		}

		SUBCASE("call a native converter (on a built-in type)") {
			static constexpr auto progStr =
				"\"123\": int\n";
			auto astPtr = ASTFromInputSemanticTC(scriptCache, progStr, data);
			auto& ast = astPtr.rootNode()[0];
			CHECK(ast.size() == 1);
			CHECK(ast[0].type() == ska::ExpressionType::INT);
		}

		SUBCASE("trying to call a converter without declaring it") {
			try {
				static constexpr auto progStr =
					"testFcty189 = function() :var do\n"
					"return {\n"
					"}\n"
					"end\n"
					"testFcty189()\n"
					"testFcty189(): int\n";
				ASTFromInputSemanticTC(scriptCache, progStr, data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{ e.what() }.find("trying to access to an undeclared field : \":int\" of") != std::string::npos);
			}
		}

		SUBCASE("trying to convert a built-in type to a custom one") {
			try {
				static constexpr auto progStr =
					"testFcty189 = function() :var do\n"
					"return {\n"
					":int do\n"
					"return 1\n"
					"end\n"
					"}\n"
					"end\n"
					"123: testFcty189()\n";
				ASTFromInputSemanticTC(scriptCache, progStr, data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{ e.what() }.find("invalid conversion from a built-in type \"int\" to type \"var testFcty189\"") != std::string::npos);
			}
		}

		SUBCASE("boolean") {
			auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi = 3 == 3\n", data);
			auto& ast = astPtr.rootNode()[0];
			CHECK(ast.size() == 1);
			CHECK(ast[0].type() == ska::ExpressionType::BOOLEAN);
		}

		SUBCASE("Cross type") {
			SUBCASE("float x float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi = 2.0\n toto = 5.2\n titi * toto\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("int x string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi = 2\n toto = \"test\"\n titi * toto\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("int x float") {
				// No implicit conversion
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 2\n toto = 5.2\n titi * toto\n", data));
			}

			SUBCASE("float x int") {
				// No implicit conversion
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 2.0\n toto = 5\n titi * toto\n", data));
			}

			SUBCASE("string x int") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi = \"tititititit\"\n toto = 6\n titi * toto\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("string = int") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = \"strrrr\"\n titi = 3\n", data));
			}

			SUBCASE("float = string") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 0.1\n titi = \"toto\"\n", data));
			}

			SUBCASE("float = string (variable)") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 0.1\n toto = \"toto\"\n titi = toto\n", data));
			}

			SUBCASE("float = float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi = 0.1\n titi = 0.2\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("float = int") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 0.1\n titi = 2\n", data));
			}

			SUBCASE("int = float") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 1\n titi = 0.2\n", data));
			}

			SUBCASE("int = string") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 7\n titi = \"123\"\n", data));
			}

			SUBCASE("float = string") {
				// No implicit conversion : a variable keeps its type
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi = 7.8\n titi = \"123\"\n", data));
			}

			SUBCASE("not an lvalue") {
				try {
					ASTFromInputSemanticTC(scriptCache, "testIf188 = 3\n lvalFunc = function() : int do return 0\n end\n lvalFunc() = testIf188\n", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("The symbol \"\" is not an lvalue, therefore cannot be assigned") != std::string::npos);
				}
			}

		}

		SUBCASE("Parameters") {
			SUBCASE("function call : string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi121 = function(test:string) do end\n titi121(\"lol\")\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::VOID);
			}


			SUBCASE("Calling a function with an argument of another type") {
				// No implicit conversion
				CHECK_THROWS(ASTFromInputSemanticTC(scriptCache, "titi129 = function(test:string) do end\n titi129(23)\n", data));
			}

			SUBCASE("constructor with 1 parameter") {
				ASTFromInputSemanticTC(scriptCache, "Joueur1 = function(nomL:string) : var do return { nom = nomL }\n end\n joueur1 = Joueur1(\"joueur 1\")\n joueur1.nom\n", data);
			}

			SUBCASE("constructor with 1 parameter same name") {
				ASTFromInputSemanticTC(scriptCache, "Joueur2 = function(nom204:string) : var do return { nom204 = nom204 }\n end\n joueur1 = Joueur2(\"joueur 1\")\n joueur1.nom204\n", data);
			}
		}

		SUBCASE("return") {
			SUBCASE("built-in") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "testReturn148 = function() : int do return 2543\n end\n value = testReturn148()\n value\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::INT);
			}

			SUBCASE("bad built-in type (no return type mentioned = void)") {
				try {
					ASTFromInputSemanticTC(scriptCache, R"script(
						testReturn148 = function() do
							return 2543
						end
						value = testReturn148()
						value
					)script", data);
					CHECK(false);
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					CHECK(std::string{e.what()}.find("bad return type : expected \"void\" on function declaration but got \"int\" on return") != std::string::npos);
				}
			}

			SUBCASE("bad custom and built-in type mix") {
				try {
					ASTFromInputSemanticTC(scriptCache, "testReturn148 = function() do return { toto = 2 }\n end\n value = testReturn148()\n value\n", data);
					CHECK(false);
				} catch (std::exception & e) {
					CHECK(std::string{e.what()}.find("bad return type : expected \"void\" on function declaration but got \"var testReturn148\" on return") != std::string::npos);
				}
			}

			SUBCASE("bad return : not on all code paths") {
				try {
					ASTFromInputSemanticTC(scriptCache, "testReturn148 = function() : int do if(true) \n return 2543\n end end\n", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("function lacks of return in one of its code path") != std::string::npos);
				}
			}

			SUBCASE("bad return : not placed in direct function-constructor") {
				try {
					ASTFromInputSemanticTC(scriptCache, "do\n return 2543\n end\n", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("bad user-defined return placing : custom return must be set in a named function-constructor") != std::string::npos);
				}
			}

			SUBCASE("code after return : no effect") {
				ASTFromInputSemanticTC(scriptCache, "testReturn148 = function() : int do return 2543\n test = 2\n end\n", data);
			}

			SUBCASE("void function return assignment") {
				constexpr auto progStr =
					"callback = function() do end\n"
					"object = callback()\n";

				try {
					ASTFromInputSemanticTC(scriptCache, progStr, data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("The symbol \"object\" cannot be declared as a void type") != std::string::npos);
				}
			}

		}

		SUBCASE("Conditions") {
			SUBCASE("bad type : not a bool") {
				try {
					ASTFromInputSemanticTC(scriptCache, "testIf188 = 3\n if(testIf188) \n end", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("expression condition is not a boolean (it's a \"int\")") != std::string::npos);
				}
			}

			SUBCASE("type OK : bool") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "testIf188 = true\n if(testIf188) \n end", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[0][0].type() == ska::ExpressionType::BOOLEAN);
				CHECK(ast[1][0].type() == ska::ExpressionType::BOOLEAN);
			}

			SUBCASE("type OK : bool indirect") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "testIf188 = 3 == 3\n if(testIf188) \n end", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::BOOLEAN);
			}

		}

		SUBCASE("Loop") {
			SUBCASE("using inner for variable successively ") {
				ASTFromInputSemanticTC(scriptCache, "for(i = 0\ni < 10\n i = i + 1)\n for(i = 0\n i < 10\n i = i + 1)\n", data);
			}
		}

		SUBCASE("Array") {

			SUBCASE("empty : explicit type") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str298 = []: string\n", data);
				auto& astArrayType = astPtr.rootNode()[0];
				CHECK(astArrayType.size() == 1);
				auto& ast = astArrayType[0];
				CHECK(ast.size() == 1);
				CHECK(ast[0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[0].type().value().size() == 1);
				CHECK(ast[0].type().value()[0] == ska::ExpressionType::STRING);
			}

			SUBCASE("empty : explicit type and add a cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str298 = []: string\n str298 = str298 + \"4\"\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::STRING));
			}


			SUBCASE("string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str152 = [\"tt\", \"titi\"]\n", data);
				auto& astArrayType = astPtr.rootNode()[0];
				CHECK(astArrayType.size() == 1);
				auto& ast = astArrayType[0];
				CHECK(ast.size() == 1);
				CHECK(ast[0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[0].type().value().size() == 1);
				CHECK(ast[0].type().value()[0] == ska::ExpressionType::STRING);
			}

			SUBCASE("string : cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str159 = [\"tt\", \"titi\"]\n str159[0]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("int : add a cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25]\n toto = toto + 4\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25, 3]\n toto = toto - 1\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell range") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25, 3]\n range = [0, 1]\n toto = toto - range\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell range, in-place value") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25, 3]\n toto = toto - [0, 1]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("double array string : cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str167 = [[0, 1], [2, 3]]\n str167[0][0]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::INT);

			}

			SUBCASE("double array string : array cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str486 = [[0, 1], [2, 3]]\n str486[0]\n str486[1][2]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[1].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[2].type() == ska::ExpressionType::INT);

			}

			SUBCASE("function call : array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "titi131 = function(test131:string[]) do end\n strArray131 = [\"lol\", \"toto\"]\n titi131(strArray131)\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
			}

			SUBCASE("filter applied on array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "array358 = [\"bup\", \"bip\", \"bap\"]\n array358 | (iteratorOnArray358) do end\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].size() == 3);
				CHECK(ast[1][1].size() == 2);
				CHECK(ast[1][1][0].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("expression-array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var186 = function() : var do\n toto = [0]\n return { toto = toto }\n end\n var186().toto[0]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[1][0].type().value()[0] == ska::ExpressionType::INT);
			}

			SUBCASE("filter applied on expression-array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var375 = function() : var do\n toto = [0]\n return { toto = toto }\n end\n var375().toto | (iteratorOnArray375) do end\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].size() == 3);
				CHECK(ast[1][1].size() == 2);
				CHECK(ast[1][1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("filter applied on array with use of iterator & index") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "array358 = [\"bup\", \"bip\", \"bap\"]\n array358 | (iteratorOnArray358, index) do iteratorOnArray358 = iteratorOnArray358 + \".\"\n index = index + 1\n end\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].size() == 3);
				CHECK(ast[1][1].size() == 2);
				CHECK(ast[1][1][0].type() == ska::ExpressionType::STRING);
			}


			SUBCASE("expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var193 = function() : var do toto = [0]\n return { toto = toto }\n end\n 2 * 3 + var193().toto[0]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var200 = function() : var do toto = [0]\n return { toto = toto }\n end\n 2 + 3 * var200().toto[0]\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("post expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var201 = function() : var do toto = [0]\n return { toto = toto }\n end\n var201().toto[0] * 3 + 2\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("post expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var202 = function() : var do toto = [0]\n return { toto = toto }\n end\n var202().toto[0] + 3 * 2\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("complex expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var203 = function() : var do toto = [0]\n return { toto = toto }\n end\n (5 + var203().toto[0] + 3 * 4) * 2\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("complex expression-array with variable used in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var203 = function() : var do toto = [0]\n return { toto = toto }\n end\n totoVar = var203().toto\n (5 + totoVar[0] + 3 * 4) * 2\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 3);
				CHECK(ast[2][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("array built-in field size") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto457 = [0]\n toto457.size()\n", data);
				auto& ast = astPtr.rootNode()[0];
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::INT);
			}

			SUBCASE("Fail") {
				SUBCASE("not an array") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "var187 = 123\n var187[0]\n", data);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(std::string{e.what()}.find("expression is not an array (it's a \"int\")") != std::string::npos);
					}
				}

				SUBCASE("array undeclared built-in field") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto457 = [0]\n toto457.turlututu\n", data);
						CHECK(false);
					} catch (std::exception & e) {
						CHECK(std::string{ e.what() }.find("trying to access an undeclared built-in field \"turlututu\" of the type \"array (int)\"") != std::string::npos);
					}
				}

				SUBCASE("array trying to assign a value to built-in method size") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto457 = [0]\n toto457.size() = 123\n", data);
						CHECK(false);
					} catch (std::exception & e) {
						CHECK(std::string{ e.what() }.find("is not an lvalue, therefore cannot be assigned") != std::string::npos);
					}
				}

				SUBCASE("operator in wrong order : ") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "str242 = [0, 1]\n str242*[0] 2\n", data);
						auto& ast = astPtr.rootNode()[0];
						CHECK(ast.size() == 2);
						CHECK(ast[1].type() == ska::ExpressionType::INT);
					} catch(std::exception& e) {
						CHECK(std::string{e.what()}.find("Unable to use operator \"*\" on types \"array\" and \"int\"") != std::string::npos);
					}
				}

				SUBCASE("syntax error ']'") {
					try {
						ASTFromInputSemanticTC(scriptCache, "str170 = [\"tt\", \"titi\"]]\n", data);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(std::string{ e.what() }.find("syntax error : bad token matching : expected \"\n\" but got \"]\""));
					}
				}

				SUBCASE("int : bad operation : operator - with string") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25, 3]\n toto = toto - \"1\"\n", data);
						auto& ast = astPtr.rootNode()[0];
						CHECK(ast.size() == 2);
						CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
					} catch(std::exception& e) {
						CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"array\" and \"string\"") != std::string::npos);
					}
				}

				SUBCASE("int : bad operation : operator - with int, reverse ordered") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [14, 25, 3]\n toto = 1 - toto\n", data);
						auto& ast = astPtr.rootNode()[0];
						CHECK(ast.size() == 2);
						CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
					} catch(std::exception& e) {
						CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"int\" and \"array\"") != std::string::npos);
					}
				}
			}
		}
	}

	SUBCASE("Fail") {

		SUBCASE("Because of undeclared symbol in expression") {
			try {
				ASTFromInputSemanticTC(scriptCache, "tti\n", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("Symbol not found : tti") != std::string::npos);
			}
		}

		SUBCASE("Because of non-matching type (variable then function)") {
			try {
				ASTFromInputSemanticTC(scriptCache, "i = 120\n i = function() do end\n", data);
				CHECK(false);
			} catch(std::exception& e) {
				CHECK(std::string{e.what()}.find("Symbol already exists : i") != std::string::npos);
			}
		}

		SUBCASE("Because of non-matching type (function then variable)") {
			try {
				ASTFromInputSemanticTC(scriptCache, "titi = function() do end\n titi = 9\n", data);
				CHECK(false);
			} catch(std::exception& e) {
				CHECK(std::string{e.what()}.find("Unable to use operator \"=\" on types \"function\" and \"int\"") != std::string::npos);
			}
		}

		SUBCASE("Reassigning function (function = function)") {
			try {
				ASTFromInputSemanticTC(scriptCache, "titi = function() do end\n titi = function(ttt:string) do end\n", data);
				CHECK(false);
			} catch(std::exception& e) {
				CHECK(std::string{e.what()}.find("Symbol already exists : titi") != std::string::npos);
			}
		}

		SUBCASE("Calling a function with wrong type arguments") {
			try {
				ASTFromInputSemanticTC(scriptCache, "titi = function(test:function) do end\n titi(23)\n", data);
				CHECK(false);
			} catch(std::exception& e) {
				CHECK(std::string{e.what()}.find("Unable to use operator \"=\" on types \"function\" and \"int\"") != std::string::npos);
			}
		}

		SUBCASE("constructor with 1 parameter + bad field access") {
			try {
				ASTFromInputSemanticTC(scriptCache, "Joueur = function(nom:string) : var do return { nom = nom }\n end\n joueur1 = Joueur(\"joueur 1\")\n joueur1.ttetetetet\n", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("trying to access to an undeclared field : \"ttetetetet\" of \"joueur1\"") != std::string::npos);
			}
		}

		SUBCASE("Array : heterogen types") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "toto = [1, 2.0]\n", data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{e.what()}.find("array has not uniform types in it : \"int\" and \"float\"" ) != std::string::npos);
			}
		}

		//TODO : move this test in TypeCross_test
		SUBCASE("Array : del a range of cell : failure bad type (double)") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache,"toto = [14.0, 25.0, 13.0, 2.0]\n toto = toto - [1.0, 2.0]\n", data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"array (float)\" and \"array (float)\"" ) != std::string::npos);
			}
		}

		SUBCASE("filter applied on expression that is not an array") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var556 = function() : var do\n toto = 0\n return { toto = toto }\n end\n var556().toto | (iterator) do end\n", data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{ e.what() }.find("invalid collection for the current filter : expected an array but got \"int toto\" instead") != std::string::npos);
			}
		}

		SUBCASE("double array string : cell") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "str167 = [[0, 1], [2.0]]\n str167[1]\n str167[1][0]\n", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("array has not uniform types in it : \"array (int)\" and \"array (float)\"" ) != std::string::npos);
			}
		}

		SUBCASE("Method call in if condition statement [direct]") {
			auto astPtr = ASTFromInputSemanticTC(scriptCache,
			"Fcty230 = function() : var do "
				"return { size = "
					"function(): int do "
						"return 0\n"
					"end"
				"}\n"
			"end\n"
			"if (Fcty230().size() > 0) \n end", data);
		}

		SUBCASE("constructor complex with contained function NOT USING the current type and calling member function with a wrong type...") {
			CHECK_THROWS_WITH(
				ASTFromInputSemanticTC(scriptCache,
				"Joueur = function(nom_:string) : var do\n"
					"puissance_ = 10\n"

					"attaquer_ = function(cible: Joueur()) do\n"
					"end\n"

					"return {\n"
						"nom = nom_\n"
						"puissance = puissance_\n"
						"pv = 100\n"
						"attaquer = attaquer_"
					"}\n"
				"end\n"
				"joueur1 = Joueur(\"joueur1\")\n"
				"joueur2 = Joueur(\"joueur2\")\n"
				"joueur1.attaquer(1)\n"
				, data), doctest::Contains("Unable to use operator \"=\" on types \"var\" and \"int\""));
		}
	}
}
