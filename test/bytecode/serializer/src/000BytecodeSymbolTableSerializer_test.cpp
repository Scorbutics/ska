#include <iostream>
#include <iomanip>
#include <sstream>
#include <doctest.h>
#include <tuple>

#include "BytecodeSerializerTest.h"
#include "Serializer/Symbol/BytecodeSymbolTableSerializer.h"
#include "Serializer/Symbol/BytecodeSymbolTableDeserializer.h"
#include "Base/Serialization/SerializerOutput.h"

#include "NodeValue/ScriptAST.h"

TEST_CASE("[BytecodeSymbolTableSerializer] 1 scope level test + type") {
	ska::SerializerValidator::DisableAbort();
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;

	{
		auto [script, data] = Serialize(
			"toto = 1\n"
		);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

	}

	{
		auto [scriptHelper, data] = Serialize("");
		auto& script = data.storage->emplaceNamed(ska::bytecode::ScriptGeneration{ std::move(scriptHelper) });

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.size() == 1);
		const auto* totoSymbolTable = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto"));
		CHECK(totoSymbolTable != nullptr);
		CHECK(totoSymbolTable->symbol() != nullptr);
		CHECK(totoSymbolTable->symbol()->type() == ska::ExpressionType::INT);
		CHECK(totoSymbolTable->symbol()->type().size() == 0);
	}
}

TEST_CASE("[BytecodeSymbolTableSerializer] 2 scopes levels test") {
	ska::SerializerValidator::DisableAbort();
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;

	{
		auto [script, data] = Serialize(
			"toto = 1\n"
			"tata = 123\n"
			"do\n"
			"tutu = 3\n"
			"titi = 12\n"
			"end\n"
			"toutou = 5\n"
		);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

	}

	{
		auto [scriptHelper, data] = Serialize("");
		auto& script = data.storage->emplaceNamed(ska::bytecode::ScriptGeneration{ std::move(scriptHelper) });

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		// Root children (by position) : toto, tata, unnamed block scope { tutu, titi }, toutou
		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.size() == 4);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tata")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toutou")) != nullptr);

		const auto* childScope = deserializedSymbolTable.root()[2];
		REQUIRE(childScope != nullptr);
		CHECK(childScope->symbol() == nullptr);
		CHECK(childScope->size() == 2);
		CHECK((*childScope)("tutu") != nullptr);
		CHECK((*childScope)("titi") != nullptr);
	}
}

TEST_CASE("[BytecodeSymbolTableSerializer] 3 scopes levels test") {
	ska::SerializerValidator::DisableAbort();
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	{
		auto [script, data] = Serialize(
			"toto = 1\n"
			"tata = 123\n"
			"do\n"
			"tutu = 3\n"
			"do\n"
			"tiutiu = 321\n"
			"end\n"
			"titi = 12\n"
			"end\n"
			"toutou = 5\n"
		);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

	}

	{
		auto [scriptHelper, data] = Serialize("");
		auto& script = data.storage->emplaceNamed(ska::bytecode::ScriptGeneration{ std::move(scriptHelper) });

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		// Root children (by position) : toto, tata, unnamed block scope { tutu, unnamed block scope { tiutiu }, titi }, toutou
		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.size() == 4);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tata")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toutou")) != nullptr);

		const auto* childScope = deserializedSymbolTable.root()[2];
		REQUIRE(childScope != nullptr);
		CHECK(childScope->symbol() == nullptr);
		CHECK(childScope->size() == 3);
		CHECK((*childScope)("tutu") != nullptr);
		CHECK((*childScope)("titi") != nullptr);

		const auto* secondLevelChildScope = (*childScope)[1];
		REQUIRE(secondLevelChildScope != nullptr);
		CHECK(secondLevelChildScope->symbol() == nullptr);
		CHECK(secondLevelChildScope->size() == 1);
		CHECK((*secondLevelChildScope)("tiutiu") != nullptr);
	}
}

// Describes a symbol table shape (name, raw type, children names), to compare an original symbol table with its deserialized version
static std::string DescribeSymbolTable(const ska::ScopedSymbolTable* table) {
	if (table == nullptr) {
		return "(null)";
	}
	auto ss = std::stringstream{};
	ss << "\"" << table->name() << "\"";
	if (table->symbol() != nullptr) {
		ss << " " << ska::ExpressionTypeSTR[static_cast<std::size_t>(table->symbol()->nativeType())];
	}
	ss << " {";
	for (const auto& child : *table) {
		ss << " " << (child == nullptr ? "(null)" : ("\"" + child->name() + "\""));
	}
	ss << " }";
	return ss.str();
}

TEST_CASE("[BytecodeSymbolTableSerializer] function var factory test + type check") {
	ska::SerializerValidator::DisableAbort();
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	std::string originalToto;
	std::string originalTotoObject;
	std::string originalTitiClass;
	{
		auto [script, data] = Serialize(
			"toto = function() : var do\n"
			"test = 3\n"
			"return { \n"
			"test = test\n"
			"tata = \"44\"\n"
			"} end\n"
			"tata = 123\n"
			"titi = toto()\n"
			"titi.test = 5\n"
		);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

		// Record the original shape, to check the deserialized symbol table against it
		const auto& originalSymbolTable = gen.program().symbols();
		const auto* toto = originalSymbolTable.lookup(ska::SymbolTableLookup::direct("toto"));
		const auto* titi = originalSymbolTable.lookup(ska::SymbolTableLookup::direct("titi"));
		REQUIRE(toto != nullptr);
		REQUIRE(titi != nullptr);
		originalToto = DescribeSymbolTable(toto);
		originalTotoObject = DescribeSymbolTable((*toto)("toto"));
		originalTitiClass = DescribeSymbolTable(titi->classTable());
	}

	{
		auto [scriptHelper, data] = Serialize("");
		auto& script = data.storage->emplaceNamed(ska::bytecode::ScriptGeneration{ std::move(scriptHelper) });

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.size() == 3);

		// Factory function "toto" : a local "test", and its returned object table (named after the factory) with fields "test" and "tata"
		const auto* totoFunction = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto"));
		REQUIRE(totoFunction != nullptr);
		const auto* totoObject = (*totoFunction)("toto");
		REQUIRE(totoObject != nullptr);
		CHECK((*totoObject)("test") != nullptr);
		CHECK((*totoObject)("tata") != nullptr);
		CHECK(DescribeSymbolTable(totoFunction) == originalToto);
		CHECK(DescribeSymbolTable(totoObject) == originalTotoObject);

		auto toto = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto"));
		auto tata = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tata"));
		auto titi = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("titi"));
		CHECK(toto != nullptr);
		CHECK(toto->symbol() != nullptr);
		CHECK(tata != nullptr);
		CHECK(titi != nullptr);
		CHECK(titi->symbol() != nullptr);

		CHECK(titi->symbol()->nativeType() == ska::ExpressionType::OBJECT);
		CHECK(DescribeSymbolTable(titi->classTable()) == originalTitiClass);

		CHECK(toto->symbol()->nativeType() == ska::ExpressionType::FUNCTION);
		CHECK(toto->symbol()->type().size() == 1);
		CHECK((toto->symbol()->type())[0] == titi->symbol()->type());
	}
}


