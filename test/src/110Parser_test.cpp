#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptHandleAST.h"

TEST_CASE("[Parser]") {
	auto scriptCache = ska::ScriptCacheAST{};

	const auto inputStr = std::string("for(i = 0\n i < 5\n i = i + 1) do lol\n \"mdr\"\n 12\n end");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();
	auto reader = ska::ScriptAST { scriptCache, "main", tokens };
	auto p = ska::StatementParser{ keywords };
	reader.parse(p, false);
	auto ast = &reader.rootNode();

	CHECK(ast->size() == 1);

	auto& scriptTree = (*ast)[0];

	CHECK(scriptTree.size() == 1);
	auto& tree = scriptTree[0];

	CHECK(tree.size() == 4);
	const auto& declaration = tree[0];
	CHECK(declaration.op() == ska::Operator::DECLARATION);
	CHECK(declaration.size() == 1);
	CHECK(declaration.has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	CHECK(declaration[0].has(ska::Token { "0", ska::TokenType::DIGIT, {} }));

	const auto& checkStatement = tree[1];
	CHECK(checkStatement.op() == ska::Operator::BINARY);
	CHECK(checkStatement.size() == 2);
	CHECK(checkStatement[0].has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	CHECK(checkStatement.has(ska::Token { "<", ska::TokenType::SYMBOL, {} }));
	CHECK(checkStatement[1].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));

	const auto& incrementStatement = tree[2];
	CHECK(incrementStatement.op() == ska::Operator::DECLARATION);
	CHECK(incrementStatement.size() == 1);
	CHECK(incrementStatement.has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	CHECK(incrementStatement[0].op() == ska::Operator::BINARY);

	const auto& loopStatementBody = tree[3];
	CHECK(loopStatementBody.op() == ska::Operator::BLOCK);
	CHECK(loopStatementBody.size() == 3);
	const auto& functionCallStatement = loopStatementBody[0];
}

ska::ScriptAST ASTFromInput(ska::ScriptCacheAST& scriptCache, const std::string& input, const ska::ReservedKeywordsPool& keywords) {
	auto t = ska::Tokenizer {keywords, input};
	auto tokens = t.tokenize();
	auto reader = ska::ScriptAST { scriptCache, "main", tokens };
	auto p = ska::StatementParser{ keywords };
	reader.parse(p, false);
	return reader;
}

TEST_CASE("Block") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = ska::ScriptCacheAST{};

	SUBCASE("Empty block statement") {
		auto astPtr = ASTFromInput(scriptCache, "do end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BLOCK);
		CHECK(tree.size() == 0);
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput(scriptCache, "do test\n end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto &tree = ast[0];
		CHECK(tree.op() == ska::Operator::BLOCK);
		CHECK(tree.size() == 1);
		CHECK(tree[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
	}

	SUBCASE("1 statement block statement + no end statement (direct end block)") {
		auto astPtr = ASTFromInput(scriptCache, "do test end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BLOCK);
		CHECK(tree.size() == 1);
		CHECK(tree[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput(scriptCache, "do test\n titi\n end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BLOCK);
		CHECK(tree.size() == 2);
		CHECK(tree[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
		CHECK(tree[1].has(ska::Token { "titi", ska::TokenType::IDENTIFIER, {}}));
	}

	SUBCASE("1 statement, then a block statement") {
		auto& ast = ASTFromInput(scriptCache, "tititi\n do test\n titi\n end", keywords).rootNode();
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::SCRIPT_OBJECT);
		CHECK(tree.size() == 2);
		CHECK(tree[1].op() == ska::Operator::BLOCK);
		CHECK(tree[1].size() == 2);
	}
}

TEST_CASE("explicit type conversion") {
	const auto keywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCacheAST{};

	auto astPtr = ASTFromInput(scriptCache, "titi: int\n", keywords);
	/*
	CHECK(astPtr.rootNode().size() == 1);

	auto& arrayTypeDecl = astPtr.rootNode()[0];
	CHECK(arrayTypeDecl.size() == 2);
	CHECK(arrayTypeDecl.op() == ska::Operator::BINARY);

	auto& arrayDecl = arrayTypeDecl[0];
	CHECK(arrayDecl.op() == ska::Operator::UNARY);
	CHECK(arrayDecl.size() == 0);


	auto& arrayExplicitConverterCall = arrayTypeDecl[1];
	CHECK(arrayExplicitConverterCall.size() == 1);
	CHECK(arrayExplicitConverterCall.op() == ska::Operator::FUNCTION_CALL);
	*/

	/*
	CHECK(arrayExplicitConverterCall.size() == 2);
	CHECK(arrayExplicitConverterCall.op() == ska::Operator::CONVERTER_CALL);

	CHECK(arrayExplicitConverterCall[0].name() == ":int");
	CHECK(arrayExplicitConverterCall[1].size() == 3);
	CHECK(arrayExplicitConverterCall[1].op() == ska::Operator::TYPE);
	*/
}

TEST_CASE("booleans") {
	const auto keywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCacheAST{};
	SUBCASE("true") {
		auto astPtr = ASTFromInput(scriptCache, "true\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::LITERAL);
		CHECK(tree.has(ska::Token{ "true", ska::TokenType::BOOLEAN, {}}));
	}

	SUBCASE("false") {
		auto astPtr = ASTFromInput(scriptCache, "false\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::LITERAL);
		CHECK(tree.has(ska::Token{ "false", ska::TokenType::BOOLEAN, {}}));
	}
}

TEST_CASE("If keyword pattern") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = ska::ScriptCacheAST{};
	SUBCASE("If only with cond and block statement") {
		auto astPtr = ASTFromInput(scriptCache, "if (test)\n end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::IF);
		CHECK(tree.size() == 2);
		CHECK(tree[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
		CHECK(tree[1].op() == ska::Operator::BLOCK);
	}

	SUBCASE("If without parenthesis") {
		auto astPtr = ASTFromInput(scriptCache, "if test\n end", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::IF);
		CHECK(tree.size() == 2);
		CHECK(tree[0].has(ska::Token{ "test", ska::TokenType::IDENTIFIER, {} }));
		CHECK(tree[1].op() == ska::Operator::BLOCK);
	}
}

TEST_CASE("function") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = ska::ScriptCacheAST{};
	SUBCASE("with 2 arguments built-in types and no return type") {
		auto astPtr = ASTFromInput(scriptCache, "f = function(titi:int, toto:string) do end\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::DECLARATION);
		const auto& treeFunc133 = tree[0];
		CHECK(treeFunc133.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(treeFunc133.size() == 2);

		CHECK(treeFunc133[0].size() == 3);
		CHECK(treeFunc133[1].size() == 0);
		//CHECK(tree[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
	}
	//TODO rework : doesn't properly work (doesn't detect a good function returning type)
	SUBCASE("with 2 return placements (early return support)") {
		auto astPtr = ASTFromInput(scriptCache, "f_parser154 = function(titi:int) : int do if(titi == 0) \n return 1\n end return 0\n end\n int_parser154 = f_parser154(1)\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() >= 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::DECLARATION);
		const auto& treeFunc157 = tree[0];
		CHECK(treeFunc157.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(treeFunc157.size() == 2);
	}

	SUBCASE("Empty statement") {
		ASTFromInput(scriptCache, "\n", keywords);
	}

	SUBCASE("Empty statement-block") {
		ASTFromInput(scriptCache, "do\nend", keywords);
	}

	SUBCASE("Empty statement-function") {
		ASTFromInput(scriptCache, "test255 = function() do \n end\n test255()\n", keywords);
	}

	SUBCASE("function with converter operator") {
		static constexpr auto progStr =
			"testFcty189 = function() :var do\n"
				"return {\n"
					":int do\n"
						"return 1\n"
					"end\n"
				"}\n"
			"end\n";
		ASTFromInput(scriptCache, progStr, keywords);
	}
}

TEST_CASE("filter") {
	const auto keywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCacheAST{};

	SUBCASE("with 1 argument") {
		auto astPtr = ASTFromInput(scriptCache, "array193 | (iterator) do end\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::FILTER);

		CHECK(tree[0].op() == ska::Operator::UNARY);

		const auto& treeArray193Declaration = tree[1];
		CHECK(treeArray193Declaration.op() == ska::Operator::FILTER_DECLARATION);
		CHECK(treeArray193Declaration.size() == 2);

		CHECK(treeArray193Declaration[0].name() == "iterator");
		CHECK(treeArray193Declaration[0].op() == ska::Operator::FILTER_PARAMETER_DECLARATION);

		CHECK(treeArray193Declaration[1].op() == ska::Operator::UNARY);
		CHECK(treeArray193Declaration[1].tokenType() == ska::TokenType::EMPTY);

		const auto& treeArray193Body = tree[2];
		CHECK(treeArray193Body.op() == ska::Operator::BLOCK);
		CHECK(treeArray193Body.size() == 0);
	}

	SUBCASE("with 2 arguments") {
		auto astPtr = ASTFromInput(scriptCache, "array193 | (iterator, index) do end\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::FILTER);
		CHECK(tree[0].op() == ska::Operator::UNARY);

		const auto& treeArray193Declaration = tree[1];
		CHECK(treeArray193Declaration.op() == ska::Operator::FILTER_DECLARATION);
		CHECK(treeArray193Declaration.size() == 2);

		CHECK(treeArray193Declaration[0].name() == "iterator");
		CHECK(treeArray193Declaration[0].op() == ska::Operator::FILTER_PARAMETER_DECLARATION);

		CHECK(treeArray193Declaration[1].name() == "index");
		CHECK(treeArray193Declaration[1].op() == ska::Operator::FILTER_PARAMETER_DECLARATION);

		const auto& treeArray193Body = tree[2];
		CHECK(treeArray193Body.op() == ska::Operator::BLOCK);
		CHECK(treeArray193Body.size() == 0);
	}

	SUBCASE("missing at least 1 argument") {
		try {
			auto astPtr = ASTFromInput(scriptCache, "array236 | () do end\n", keywords);
			CHECK(false);
		} catch (std::runtime_error & e) {
			CHECK(std::string{ e.what() }.find("syntax error : bad token matching : expected \"a token with type \"IDENTIFIER\"\" but got \")\"") != std::string::npos);
		}
	}

	SUBCASE("missing body") {
		try {
			auto astPtr = ASTFromInput(scriptCache, "array240 | (iterator) \n", keywords);
			CHECK(false);
		} catch (std::runtime_error & e) {
			CHECK(std::string{ e.what() }.find("bad token detected (expected a block start token)") != std::string::npos);
		}
	}

	SUBCASE("missing filter declaration + body") {
		try {
			auto astPtr = ASTFromInput(scriptCache, "array254 | \n", keywords);
			CHECK(false);
		} catch (std::runtime_error & e) {
			CHECK(std::string{ e.what() }.find("syntax error : bad token matching : expected \"(\" but got") != std::string::npos);
		}
	}

}

TEST_CASE("array : explicit type") {
	const auto keywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCacheAST{};

	auto astPtr = ASTFromInput(scriptCache, "[]: string\n", keywords);
	CHECK(astPtr.rootNode().size() == 1);

	auto& arrayTypeDecl = astPtr.rootNode()[0][0];
	CHECK(arrayTypeDecl.size() == 2);
	CHECK(arrayTypeDecl.op() == ska::Operator::ARRAY_TYPE_DECLARATION);

	auto& arrayDecl = arrayTypeDecl[0];
	CHECK(arrayDecl.op() == ska::Operator::ARRAY_DECLARATION);
	CHECK(arrayDecl.size() == 0);

	auto& arrayExplicitType = arrayTypeDecl[1];
	CHECK(arrayExplicitType.size() == 3);
	CHECK(arrayExplicitType.op() == ska::Operator::TYPE);
}


TEST_CASE("User defined object") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = ska::ScriptCacheAST{};

	SUBCASE("constructor with 1 parameter") {

		auto astPtr = ASTFromInput(scriptCache, "Joueur = function(nom:string) : var do return { nom = nom }\n end\n joueur1 = Joueur(\"joueur 1\")\n joueur1.nom\n", keywords);
		CHECK(astPtr.rootNode().size() >= 1);
		auto& tree = astPtr.rootNode()[0];
		CHECK(tree.size() == 3);
		CHECK(tree.op() == ska::Operator::SCRIPT_OBJECT);

		auto& varJoueurNode = tree[0];
		CHECK(varJoueurNode.size() == 1);
		CHECK(varJoueurNode.op() == ska::Operator::DECLARATION);
		const auto& astFunc154 = varJoueurNode[0];
		CHECK(astFunc154.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc154.size() == 2);
		const auto& astFuncParameters154 = astFunc154[0];

		//Joueur factory block is inside "astFuncParameters154"
		CHECK(astFuncParameters154.size() == 2);

		//Checks the parameters
		CHECK(astFuncParameters154[0].size() == 1);
		CHECK(astFuncParameters154[0][0].size() == 3);
		CHECK(astFuncParameters154[0][0][0].has(keywords.pattern<ska::TokenGrammar::STRING>()));

		//Checks the return type
		CHECK(astFuncParameters154[astFuncParameters154.size() - 1][0].has(keywords.pattern<ska::TokenGrammar::VARIABLE>()));

		//Checks the function body
		CHECK(astFunc154[1].size() == 1);

		//Checks the return
		const auto& returnNode = astFunc154[1][0];
		CHECK(returnNode.op() == ska::Operator::RETURN);
		CHECK(returnNode.size() == 1);

		//Checks the object in the return
		const auto& userDefinedObjectNode = returnNode[0];
		CHECK(userDefinedObjectNode.op() == ska::Operator::USER_DEFINED_OBJECT);
		CHECK(userDefinedObjectNode.size() == 1);

		const auto& returnNomNode = userDefinedObjectNode[0];
		CHECK(returnNomNode.size() == 1);
		CHECK(returnNomNode.has(ska::Token { "nom", ska::TokenType::IDENTIFIER, {}}));

		//Checks the variable declaration and the function call
		const auto& varJoueur1Node = tree[1];
		CHECK(varJoueur1Node.op() == ska::Operator::DECLARATION);
		CHECK(varJoueur1Node.has(ska::Token { "joueur1", ska::TokenType::IDENTIFIER, {}} ));

		//Checks the field access
		const auto& nomJoueur1FieldNode = tree[2];
		CHECK(nomJoueur1FieldNode.op() == ska::Operator::FIELD_ACCESS);
		CHECK(nomJoueur1FieldNode.size() == 2);
		CHECK(nomJoueur1FieldNode[0].has(ska::Token { "joueur1", ska::TokenType::IDENTIFIER, {}} ));
		CHECK(nomJoueur1FieldNode[1].has(ska::Token{ "nom", ska::TokenType::IDENTIFIER, {}}));

	}

	SUBCASE("with badly configured scope") {
		auto astPtr = ASTFromInput(scriptCache ,R"script(
Wrapper = function(i: int): var do
	return {
		value = 1
		more_value = value + 1
	}
end
)script", keywords);
		CHECK(astPtr.rootNode().size() == 1);
		auto& tree = astPtr.rootNode()[0];
		CHECK(tree.size() == 1);
		CHECK(tree.op() == ska::Operator::SCRIPT_OBJECT);

		auto& varJoueurNode = tree[0];
		CHECK(varJoueurNode.size() == 1);
		CHECK(varJoueurNode.op() == ska::Operator::DECLARATION);
		const auto& astFunc154 = varJoueurNode[0];
		CHECK(astFunc154.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc154.size() == 2);
		const auto& astFuncParameters154 = astFunc154[0];
	}

}

TEST_CASE("Expression and priorities") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = ska::ScriptCacheAST{};
	SUBCASE("Simple mul") {
		auto astPtr = ASTFromInput(scriptCache, "5 * 2\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "*", ska::TokenType::SYMBOL, {}}));
		CHECK(tree[0].has(ska::Token { "5", ska::TokenType::DIGIT, {}}));
		CHECK(tree[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Syntax error : not an expression") {
		auto toCheck = true;
		try {
			ASTFromInput(scriptCache, "5 * 2", keywords);
		} catch (std::exception& e) {
			toCheck = false;
		}
		CHECK(!toCheck);
	}

	SUBCASE("Syntax error : no existing operator") {
		try {
			ASTFromInput(scriptCache, "5 ' 3\n", keywords);
			CHECK(false);
		} catch(std::exception& e) {
			CHECK(true);
		}
	}

	SUBCASE("Simple div") {
		auto astPtr = ASTFromInput(scriptCache, "5 / 2\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "/", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(tree[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Simple add") {
		auto astPtr = ASTFromInput(scriptCache, "5 + 2\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(tree[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Simple sub") {
		auto astPtr = ASTFromInput(scriptCache, "5 - 2\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "-", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(tree[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Priorization with mul before add") {
		auto astPtr = ASTFromInput(scriptCache, "5 * 2 + 4\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));
		CHECK(tree[0].op() == ska::Operator::BINARY);
		const auto& innerOp = tree[0];
		CHECK(innerOp[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
	}

	SUBCASE("Priorization with mul after add") {
		auto astPtr = ASTFromInput(scriptCache, "5 + 2 * 4\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(tree[1].op() == ska::Operator::BINARY);
		const auto& innerOp = tree[1];
		CHECK(innerOp[0].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
	}


	SUBCASE("Priorization with mul after add with parenthesis") {
		auto astPtr = ASTFromInput(scriptCache, "(5 + 2) * 4\n", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.size() == 1);
		auto& tree = ast[0];
		CHECK(tree.op() == ska::Operator::BINARY);
		CHECK(tree.size() == 2);
		CHECK(tree.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
		CHECK(tree[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));
		CHECK(tree[0].op() == ska::Operator::BINARY);
		const auto& innerOp = tree[0];
		CHECK(innerOp[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
	}

}
