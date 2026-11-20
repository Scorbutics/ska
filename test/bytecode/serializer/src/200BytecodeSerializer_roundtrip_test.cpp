#include <doctest.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include "Config/LoggerConfigLang.h"

#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "BytecodeSerializerTest.h"
#include "BytecodeScriptCompare.h"
#include "Base/Serialization/SerializerValidator.h"

// Behavioral round trip : a program is generated and run, all its scripts are serialized,
// then deserialized into a fresh cache and run again.
// The deserialized bytecode must be identical (scripts matched by name), and running it must produce the same result.

namespace {
	struct RoundTripResult {
		std::string originalResult;
		std::string deserializedResult;
		std::vector<std::string> failedScripts;
		std::vector<std::string> differentScripts;
		std::size_t originalScripts = 0;
		std::size_t deserializedScripts = 0;
	};

	RoundTripResult RoundTrip(const std::string& progStr) {
		ska::SerializerValidator::DisableAbort();
		auto streams = std::unordered_map<std::string, std::stringstream>{};
		auto result = RoundTripResult{};

		auto [scriptIn, dataIn] = Serialize(progStr);
		auto interpreterIn = ska::bytecode::Interpreter{ *dataIn.parser, *dataIn.generator, dataIn.reservedKeywords };
		auto& genIn = dataIn.generator->generate(*dataIn.storage, std::move(scriptIn));
		const auto mainIdIn = genIn.id();
		result.originalResult = interpreterIn.interpret(mainIdIn, *dataIn.storage)->variable(mainIdIn).convertString();

		const auto serialized = dataIn.serializer->serialize(*dataIn.storage, [&streams](const std::string& scriptName) -> std::ostream& {
			return streams[scriptName];
		});
		REQUIRE(serialized);

		auto [scriptOut, dataOut] = Serialize("");
		result.failedScripts = dataOut.serializer->deserialize(*dataOut.storage, "main", [&streams](const std::string& scriptName) -> std::istream& {
			return streams[scriptName];
		});

		result.originalScripts = dataIn.storage->size();
		result.deserializedScripts = dataOut.storage->size();
		for (std::size_t index = 0; index < dataIn.storage->size(); index++) {
			const auto& original = dataIn.storage->at(index);
			const auto* deserialized = dataOut.storage->atOrNull(original.name());
			auto difference = std::string{ "missing script" };
			if (deserialized == nullptr || !SameScriptAcrossCaches(*dataIn.storage, original, *dataOut.storage, *deserialized, &difference)) {
				result.differentScripts.push_back(original.name());
				auto ss = std::stringstream{};
				ss << "Script \"" << original.name() << "\" differs after round trip (" << difference << ").\nOriginal :\n" << original;
				if (deserialized != nullptr) {
					ss << "\nDeserialized :\n" << *deserialized;
				}
				const auto printExports = [&ss](const ska::bytecode::ScriptGeneration& script) {
					ss << "\nExports :";
					for (const auto& exported : script.exportedSymbols()) {
						if (exported.has_value()) {
							ss << " " << (exported->symbol != nullptr ? exported->symbol->name() : "?") << "=" << exported->value;
						}
					}
				};
				printExports(original);
				if (deserialized != nullptr) {
					printExports(*deserialized);
				}
				MESSAGE(ss.str());
			}
		}

		auto interpreterOut = ska::bytecode::Interpreter{ *dataOut.parser, *dataOut.generator, dataOut.reservedKeywords };
		const auto mainIdOut = dataOut.storage->id("main");
		result.deserializedResult = interpreterOut.interpret(mainIdOut, *dataOut.storage)->variable(mainIdOut).convertString();
		return result;
	}

	void CheckRoundTrip(const std::string& progStr, const std::string& expected) {
		const auto result = RoundTrip(progStr);
		CHECK(result.failedScripts.empty());
		CHECK(result.differentScripts.empty());
		CHECK(result.deserializedScripts == result.originalScripts);
		// The original run must give the expected result, otherwise the test does not prove anything
		CHECK(result.originalResult == expected);
		CHECK(result.deserializedResult == result.originalResult);
	}
}

TEST_CASE("[BytecodeSerializer round trip] literals, operations and type conversions") {
	CheckRoundTrip(R"script(
		i = 3 + 4 * 2 - 1
		f = 1.5 * 2.0
		s = "a" + "b"
		fromInt = i : string
		fromString = "12" : int
		out = s + "/" + fromInt + "/" + (fromString + i) + "/" + (f : string)
	)script", "ab/10/22/3.000000");
}

TEST_CASE("[BytecodeSerializer round trip] control flow and arrays") {
	CheckRoundTrip(R"script(
		total = 0
		for(i = 0
		i < 5
		i = i + 1) do
			total = total + i
		end
		label = "small"
		if (total > 5)
			label = "big"
		else
			label = "none"
		end
		values = [1, 2, 3] + 4
		sum = 0
		values | (value, index) do
			sum = sum + value * index
		end
		out = label + "/" + total + "/" + values.size() + "/" + sum
	)script", "big/10/4/20");
}

TEST_CASE("[BytecodeSerializer round trip] functions and callbacks") {
	CheckRoundTrip(R"script(
		add = function(a: int, b: int): int do
			return a + b
		end
		counter = 0
		increment = function() do
			counter = counter + 1
		end
		callTwice = function(callback: increment) do
			callback()
			callback()
		end
		callTwice(increment)
		out = "" + add(2, 3) + "/" + counter
	)script", "5/2");
}

TEST_CASE("[BytecodeSerializer round trip] factory objects : fields, methods, field writes, parameter named like a field") {
	CheckRoundTrip(R"script(
		Position = function(x: int, y: int): var do
			return {
				x = x
				y = y
				sum = function(): int do
					return x + y
				end
			}
		end
		first = Position(1, 2)
		second = Position(10, 20)
		before = first.x
		first.x = 5
		out = "" + before + "/" + first.x + "/" + first.y + "/" + second.x + "/" + second.sum()
	)script", "1/5/2/10/30");
}

TEST_CASE("[BytecodeSerializer round trip] closures : distinct instances, setter, getter on an aliased field") {
	CheckRoundTrip(R"script(
		Box = function(value_: string): var do
			internal = value_
			return {
				value = value_
				get = function(): string do
					return value
				end
				getInternal = function(): string do
					return internal
				end
				setInternal = function(v: string) do
					internal = v
				end
			}
		end
		a = Box("a")
		b = Box("b")
		a.setInternal("c")
		b.value = "d"
		out = a.getInternal() + b.getInternal() + a.get() + b.get()
	)script", "cbad");
}

TEST_CASE("[BytecodeSerializer round trip] self referencing factory") {
	CheckRoundTrip(R"script(
		JSON = function(v : string) : var do
			child = [] : JSON()
			return {
				Size = function() : int do
					return child.size()
				end
				Get = function(key : int) : JSON() do
					return child[key]
				end
				Add = function(c : JSON()) do
					child = child + c
				end
				Value = function() : string do
					return v
				end
			}
		end
		json = JSON("toto")
		json.Add(JSON("titi"))
		out = json.Get(0).Value() + json.Value() + json.Size()
	)script", "tititoto1");
}

TEST_CASE("[BytecodeSerializer round trip] block scopes and more than 10 top level symbols") {
	CheckRoundTrip(R"script(
		do
		end
		v0 = 0
		v1 = 1
		v2 = 2
		do
			nested = 3
			do
				deeper = nested + 1
			end
		end
		v3 = 3
		v4 = 4
		v5 = 5
		v6 = 6
		v7 = 7
		v8 = 8
		v9 = 9
		v10 = 10
		v11 = 11
		out = "" + (v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + v10 + v11)
	)script", "66");
}

TEST_CASE("[BytecodeSerializer round trip] imported external script") {
	CheckRoundTrip(
		"Character = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"player = Character.build(\"Player\")\n"
		"enemy = Character.default\n"
		"enemy.age = 99\n"
		"out = player.name + \"/\" + player.age + \"/\" + enemy.name + \"/\" + Character.default.age\n",
		"Player/10/Default/99");
}
