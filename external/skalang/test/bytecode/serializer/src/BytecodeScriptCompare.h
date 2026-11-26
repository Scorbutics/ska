#pragma once
#include <string>
#include <sstream>
#include <variant>
#include "Generator/Value/BytecodeScriptCache.h"
#include "Generator/Value/BytecodeScriptGeneration.h"

// Compares a generated script with its deserialized version, living in another cache.
// Script ids may differ between both caches (ex : a bound script declared before "main" in the original one),
// so operands refering to a script are compared through the name of this script.

inline std::string ScriptNameOfId(const ska::bytecode::ScriptCache& cache, std::size_t scriptId) {
	return cache.exist(scriptId) ? cache.at(scriptId).name() : ("#" + std::to_string(scriptId));
}

inline bool SameOperandAcrossCaches(const ska::bytecode::ScriptCache& leftCache, const ska::bytecode::Operand& left,
	const ska::bytecode::ScriptCache& rightCache, const ska::bytecode::Operand& right) {
	// Same rule as Operand::operator== : strings are compared by content only
	// (a string literal is deserialized as a MAGIC operand, while it was generated as a PURE one)
	if (std::holds_alternative<ska::StringShared>(left.content())) {
		return left == right;
	}
	if (left.type() != right.type()) {
		return false;
	}
	const auto* leftRef = std::get_if<ska::ScriptVariableRef>(&left.content());
	const auto* rightRef = std::get_if<ska::ScriptVariableRef>(&right.content());
	if (left.empty() || leftRef == nullptr || rightRef == nullptr) {
		return left == right;
	}
	if (ScriptNameOfId(leftCache, leftRef->script) != ScriptNameOfId(rightCache, rightRef->script)) {
		return false;
	}
	if (left.type() == ska::bytecode::OperandType::BIND_SCRIPT) {
		// "variable" is also a script id (the referenced script)
		return ScriptNameOfId(leftCache, leftRef->variable) == ScriptNameOfId(rightCache, rightRef->variable);
	}
	return leftRef->variable == rightRef->variable;
}

// "difference" (optional) receives a description of the first difference found
inline bool SameScriptAcrossCaches(const ska::bytecode::ScriptCache& leftCache, const ska::bytecode::ScriptGeneration& left,
	const ska::bytecode::ScriptCache& rightCache, const ska::bytecode::ScriptGeneration& right, std::string* difference = nullptr) {
	const auto differ = [difference](auto&&... parts) {
		if (difference != nullptr) {
			auto ss = std::stringstream{};
			(ss << ... << parts);
			*difference = ss.str();
		}
		return false;
	};
	if (left.size() != right.size()) {
		return differ("instruction count ", left.size(), " != ", right.size());
	}
	if (left.exportedSymbols().size() != right.exportedSymbols().size()) {
		return differ("exports count ", left.exportedSymbols().size(), " != ", right.exportedSymbols().size());
	}
	std::size_t index = 0;
	auto rightInstruction = right.begin();
	for (const auto& leftInstruction : left) {
		if (leftInstruction.command() != rightInstruction->command() ||
			!SameOperandAcrossCaches(leftCache, leftInstruction.dest(), rightCache, rightInstruction->dest()) ||
			!SameOperandAcrossCaches(leftCache, leftInstruction.left(), rightCache, rightInstruction->left()) ||
			!SameOperandAcrossCaches(leftCache, leftInstruction.right(), rightCache, rightInstruction->right())) {
			return differ("instruction ", index, " : ", leftInstruction, " != ", *rightInstruction);
		}
		++rightInstruction;
		++index;
	}
	auto rightExport = right.exportedSymbols().begin();
	for (const auto& leftExport : left.exportedSymbols()) {
		if (leftExport.has_value() != rightExport->has_value() ||
			(leftExport.has_value() && !SameOperandAcrossCaches(leftCache, leftExport->value, rightCache, (*rightExport)->value))) {
			return differ("export ", (leftExport.has_value() && leftExport->symbol != nullptr ? leftExport->symbol->name() : "?"));
		}
		++rightExport;
	}
	return true;
}
