#pragma once

#include "BytecodeSerializerDataTestContainer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "NodeValue/ScriptHandleAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

static const auto reservedKeywords = ska::ReservedKeywordsPool{};
static auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
static std::vector<ska::Token> tokens;
static auto readerI = std::unique_ptr<ska::ScriptAST>{};
static auto typeCrosserI = ska::TypeCrosser{};

struct BytecodeSerializerTest;

static void ASTFromInputBytecodeSerializerNoParse(const std::string& input, BytecodeSerializerDataTestContainer& data) {
  tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
  tokens = tokenizer->tokenize();
	readerI = std::make_unique<ska::ScriptAST>(data.storage->astCache, "main", tokens);

	data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords);
	data.serializer = std::make_unique<ska::bytecode::Serializer>();
}

static std::pair<ska::bytecode::ScriptGenerationHelper, BytecodeSerializerDataTestContainer> Serialize(const std::string& input) {
	auto data = BytecodeSerializerDataTestContainer{};
	ASTFromInputBytecodeSerializerNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationHelper, BytecodeSerializerDataTestContainer>(ska::bytecode::ScriptGenerationHelper{*data.storage, *readerI }, std::move(data));
}

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};
