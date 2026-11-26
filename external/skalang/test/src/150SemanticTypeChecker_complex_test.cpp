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
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "NodeValue/ScriptHandleAST.h"

ska::ScriptAST ASTFromInputSemanticComplexTC(ska::ScriptCacheAST& scriptCache, const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::ScriptAST {scriptCache, "main", tokens };
	auto typeCrosser = ska::TypeCrosser{};

	data.parser = std::make_unique<ska::StatementParser> ( reservedKeywords );
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosser);
	reader.parse(*data.parser);
	return reader;
}

TEST_CASE("[SemanticTypeChecker Complex]") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

	SUBCASE("use of inline empty array declaration and as return value of function") {
		ASTFromInputSemanticComplexTC(scriptCache,
		"Get = function() : string[] do\n"
			"return[]: string\n"
		"end\n"
		, data);
	}

	SUBCASE("constructor with function field accessed through variable") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"JoueurClass = function(nom:string) : var do "
				"attaquer = function(degats:int) do\n"
				"end\n"

				"return {"
					"attaquer = attaquer"
				"}\n"
			"end\n"
			"joueur1 = JoueurClass(\"joueur1Nom\")\n"
			"joueur1.attaquer(10)\n"
			, data);
	}


	SUBCASE("constructor with integer field accessed through function call and used in expression") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"JoueurClass = function(nom:string) : var do "
				"test74 = 123\n"

				"return {"
					"test = test74"
				"}\n"
			"end\n"
	"(5 + JoueurClass(\"joueur1Nom\").test + 3 * 4) * 2\n"
			, data);
	}

	SUBCASE("constructor complex with contained function NOT USING the current type BUT mentioning it...") {
		ASTFromInputSemanticComplexTC(scriptCache,
		"JoueurClass = function(nom:string) : var do "
			"puissance = 10\n"

			"attaquer = function(cible:JoueurClass()) do\n"
			"end\n"

			"return {"
				"nom= nom\n"
				"puissance = puissance\n"
				"pv = 100\n"
				"attaquer = attaquer\n"
			"}\n"
		"end\n"
		"joueur1 = JoueurClass(\"joueur1Nom\")\n"
		"joueur2 = JoueurClass(\"joueur2Nom\")\n"
		"joueur1.attaquer(joueur2)\n"
			, data);
	}

	SUBCASE("class with internal data used in public function") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"Stats = function() : var do\n"
				"pointsDeVie = 100\n"
				"blesser = function(degats:int) do\n"
					"pointsDeVie = pointsDeVie - degats\n"
				"end\n"
				"return {"
					"blesser = blesser"
				"}\n"
			"end\n"
			"JoueurClass = function(nom:string) : var do "
			"stats = Stats()\n"

			"attaquer = function(degats:int) do\n"
			"stats.blesser(degats)\n"
			"end\n"

			"return {"
			"attaquer = attaquer\n"
			"stats = stats\n"
			"}\n"
			"end\n"
			"joueur1 = JoueurClass(\"joueur1Nom\")\n"
			"joueur1.attaquer(1)\n"
			"joueur1.stats.blesser(1)\n"
			, data);
	}

	SUBCASE("class with internal data used in public function : not same names") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"Stats = function() : var do\n"
			"pointsDeVie = 100\n"
			"blesser = function(degats:int) do\n"
			"pointsDeVie = pointsDeVie - degats\n"
			"end\n"
			"return {"
			"blesser = blesser"
			"}\n"
			"end\n"
			"JoueurClass = function(nom:string) : var do "
			"stats_ = Stats()\n"

			"attaquer_ = function(degats:int) do\n"
			"stats_.blesser(degats)\n"
			"end\n"

			"return {"
			"attaquer = attaquer_\n"
			"stats = stats_\n"
			"}\n"
			"end\n"
			"joueur1 = JoueurClass(\"joueur1Nom\")\n"
			"joueur1.attaquer(1)\n"
			"joueur1.stats.blesser(1)\n"
			, data);
	}

	SUBCASE("class with internal data used in public function : with a built-in between functions") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"Stats = function() : var do\n"
			"pointsDeVie = 100\n"
			"blesser = function(degats:int) do\n"
			"pointsDeVie = pointsDeVie - degats\n"
			"end\n"
			"return {"
			"blesser = blesser"
			"}\n"
			"end\n"
			"test = 12\n"
			"JoueurClass = function(nom:string) : var do "
			"stats_ = Stats()\n"

			"attaquer_ = function(degats:int) do\n"
			"stats_.blesser(degats)\n"
			"end\n"

			"return {"
			"attaquer = attaquer_\n"
			"stats = stats_\n"
			"}\n"
			"end\n"
			"joueur1 = JoueurClass(\"joueur1Nom\")\n"
			"joueur1.attaquer(1)\n"
			"joueur1.stats.blesser(1)\n"
			, data);
	}

	SUBCASE("class with internal data used in public function 2") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"Stats = function() : var do\n"
			"pointsDeVie = 100\n"
			"blesser = function(degats:int) do\n"
			"pointsDeVie = pointsDeVie - degats\n"
			"end\n"
			"return {"
			"blesser = blesser\n"
			"pdv = pointsDeVie\n"
			"}\n"
			"end\n"
			"JoueurClass = function(nom:string) : var do "
			"stats = Stats()\n"

			"attaquer = function(statsDegats:Stats(), test:int) do\n"
			"stats.blesser(statsDegats.pdv)\n"
			"end\n"

			"return {"
			"attaquer = attaquer"
			"}\n"
			"end\n"
			"joueur1 = JoueurClass(\"joueur1Nom\")\n"
			"statsDeg = Stats()\n"
			"joueur1.attaquer(statsDeg, 0)\n"
			, data);
	}

	SUBCASE("after field access, not an lvalue") {
		try {
			ASTFromInputSemanticComplexTC(scriptCache,
				"lvalFunc137 = function() : var do\n"
					"test137_ = function() : int do \n return 0\n end\n"
					"return { test = test137_}\n"
				"end\n"
				"object = lvalFunc137()\n"
				"object.test() = 1234\n", data);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("The symbol \"\" is not an lvalue, therefore cannot be assigned") != std::string::npos);
		}
	}

	SUBCASE("assign 2 differents object types") {
		try {
			ASTFromInputSemanticComplexTC(scriptCache,
				"lvalFunc160 = function() : var do\n"
				"return {}\n"
				"end\n"
				"lvalFunc163 = function() : var do\n"
				"return {}\n"
				"end\n"
				"object = lvalFunc160()\n"
				"object = lvalFunc163()\n", data);
			CHECK(false);
		} catch (std::exception & e) {
			CHECK(std::string{e.what()}.find("The symbol \"object\" has already been declared as \"var lvalFunc160\" but is now wanted to be \"var lvalFunc163\"") != std::string::npos);
		}
	}

	SUBCASE("assign new field object instead of created one") {
		ASTFromInputSemanticComplexTC(scriptCache,
		R"script(
			PositionFcty = function(x: int, y: int): var do
				return {
					x = x
					y = y
				}
			end
			lvalFunc244 = function(i: int) : var do
				return { pos = PositionFcty(i, i*2) }
			end
			object = lvalFunc244(3)
			object.pos = PositionFcty(4, 3)
			)script", data);
		CHECK(true);
	}

	SUBCASE("Function 0 parameter creating custom object but forget to use it as a factory (direct use of function)") {
		CHECK_THROWS_WITH(ASTFromInputSemanticComplexTC(scriptCache, "Dummy = function() : var do\n return { data= 3 }\n end\n Dummy.data\n ", data),
		doctest::Contains("the variable \"Dummy\" is not registered as an object but as a \"function (var"));
	}

	SUBCASE("return a concrete custom type") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"lvalFunc185 = function() : var do\n"
			"return { test = 14 }\n"
			"end\n"
			"lvalFunc188 = function() : lvalFunc185() do\n"
			"return lvalFunc185()\n"
			"end\n"
			"object = lvalFunc188()\n"
			"object.test = 1234\n", data);
	}

	SUBCASE("return a concrete custom type with namespace") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"Character = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
			"lvalFunc188 = function() : Character::build() do\n"
				"return Character.build(\"t\")\n"
			"end\n"
			"object = lvalFunc188()\n"
			"object.age = 1234\n", data);
	}

	SUBCASE("using a function as a parameter") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"lvalFunc206 = function() : var do\n"
				"return { test = 14 }\n"
			"end\n"
			"lvalFunc209 = function(toto: lvalFunc206) : lvalFunc206() do\n"
				"return toto()\n"
			"end\n"
			"object = lvalFunc209(lvalFunc206)\n"
			"object.test = 1234\n", data);
	}

	SUBCASE("using a callback function as a parameter without using the source type (function type compatibility)") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"lvalFunc218 = function() do end\n"
			"lvalFunc219 = function(toto: lvalFunc218) do\n"
			"	toto()\n"
			"end\n"
			"callback = function() do end\n"
			"lvalFunc219(callback)\n", data);
	}

	SUBCASE("for with empty statement in if") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"size = function() : int do return 10\n end\n"
			"if (size() > 0) \n"
				"for(i = 0\n i < size()\n i = i + 1)\n"
			"end\n", data);
	}
}
