#include <doctest.h>
#include <sstream>
#include "BytecodeCompare.h"
#include "Generator/Value/BytecodeScriptGeneration.h"

static std::string HandleScriptSuffix(std::size_t scriptId, std::string str) {
    if (str.empty() || (str[0] != 'V' && str[0] != 'R' && str[0] != 'L' && str[0] != 'U')) {
        return str;
    }
    const auto dotPos = str.find_last_of(':');
    if (dotPos == std::string::npos) {
        return str + ":" + std::to_string(scriptId);
    }
    return str;
}

static std::string StripScriptSuffix(std::size_t scriptId, std::string str) {
    const auto suffix = ":" + std::to_string(scriptId);
    if (!str.empty() && (str[0] == 'V' || str[0] == 'R' || str[0] == 'L' || str[0] == 'U') && str.size() > suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return str.substr(0, str.size() - suffix.size());
    }
    return str;
}

static void PrintPart(std::ostream& os, const char* command, const std::string& dest, const std::string& left, const std::string& right) {
    os << "\t\t{ska::bytecode::Command::" << command << ", \"" << dest << "\"";
    if (!left.empty() || !right.empty()) { os << ", \"" << left << "\""; }
    if (!right.empty()) { os << ", \"" << right << "\""; }
    os << "},\n";
}

void BytecodeCompare(const ska::bytecode::ScriptGeneration& scriptGeneration, std::vector<ska::bytecode::BytecodePart> expected) {
  auto index = std::size_t {0};
  auto allEqual = scriptGeneration.size() == expected.size();
  CHECK(scriptGeneration.size() == expected.size());
  for(const auto& r : scriptGeneration) {
	const auto equality =
  	index < expected.size() &&
  	r.command() == expected[index].command &&
  	r.dest().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].dest) &&
  	r.left().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].left) &&
  	r.right().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].right);
	CHECK(equality);
	allEqual = allEqual && equality;
	index++;
  }

  if (!allEqual) {
	// Dump both versions using the test initializer syntax, to ease the review of differences
	auto ss = std::stringstream {};
	ss << "Actual bytecode :\n";
	for (const auto& r : scriptGeneration) {
		PrintPart(ss, ska::bytecode::CommandSTR[static_cast<std::size_t>(r.command())],
			StripScriptSuffix(scriptGeneration.id(), r.dest().toString()),
			StripScriptSuffix(scriptGeneration.id(), r.left().toString()),
			StripScriptSuffix(scriptGeneration.id(), r.right().toString()));
	}
	ss << "Expected bytecode :\n";
	for (const auto& e : expected) {
		PrintPart(ss, ska::bytecode::CommandSTR[static_cast<std::size_t>(e.command)], e.dest, e.left, e.right);
	}
	MESSAGE(ss.str());
  }
}
