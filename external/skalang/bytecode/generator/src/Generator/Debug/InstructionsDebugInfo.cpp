#include <fstream>
#include <iostream>
#include <iomanip>
#include "Runtime/Value/PlainMemoryTable.h"
#include "InstructionsDebugInfo.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Generator/Value/BytecodeScriptGeneration.h"
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeScriptCache.h"

ska::bytecode::InstructionsDebugInfo::InstructionsDebugInfo(ScriptGeneration& generation, std::size_t columnWidth) :
	m_columnWidth(columnWidth) {
	auto scriptFile = std::ifstream {generation.name()};
	if (!scriptFile.fail()) {
		m_scriptFileContent = std::string {
			(std::istreambuf_iterator<char>(scriptFile)),
			(std::istreambuf_iterator<char>())
		};
	}
}

ska::bytecode::InstructionsDebugInfo::InstructionsDebugInfo(std::string scriptData, std::size_t columnWidth) :
	m_scriptFileContent(std::move(scriptData)),
	m_columnWidth(columnWidth) {
}

static void PrintInstructions(std::vector<std::string>& stream, std::size_t tokenIndex, std::deque<ska::bytecode::Instruction>& instruction) {
	while (!instruction.empty() && (tokenIndex >= instruction.front().dest().position().rawIndex
		|| instruction.front().left().position().rawIndex != 0 && tokenIndex >= instruction.front().left().position().rawIndex
		|| instruction.front().right().position().rawIndex != 0 && tokenIndex >= instruction.front().right().position().rawIndex
		)) {
		std::stringstream ss; ss << instruction.front();
		stream.push_back(ss.str());
		instruction.pop_front();
	}
}

static void PrintTokenAndInstructionLinesInTwoColumns(std::ostream& stream, std::size_t fixedWidth, std::vector<std::stringstream>tokenLinesSerialized, std::vector<std::vector<std::string>>instructionsLinesSerialized) {
	for (auto index = std::size_t{ 0 }; index < tokenLinesSerialized.size(); index++) {
		auto tokenStr = tokenLinesSerialized[index].str();
		const auto& currentInstructions = instructionsLinesSerialized[index];

		stream << std::setw(fixedWidth) << std::left << std::move(tokenStr) << (currentInstructions.empty() ? "" : "|" + currentInstructions[0]) << std::endl;
		for (auto lineIndex = 1; lineIndex < currentInstructions.size(); lineIndex++) {
			stream << std::setw(fixedWidth) << "" << '|' << currentInstructions[lineIndex] << std::endl;
		}

		if (currentInstructions.size() > 1) {
			stream << std::setfill('.') << std::setw(fixedWidth) << "" << std::setfill(' ') << std::endl;
		}

	}
}

static void EndTokenAndInstructionLines(std::vector<std::stringstream>& tokenLinesSerialized, std::vector<std::vector<std::string>>& instructionsLinesSerialized) {
	instructionsLinesSerialized.emplace_back();
	tokenLinesSerialized.emplace_back();
}

static void PrintDequeInstructions(std::ostream& stream, std::size_t fixedWidth, const std::string& scriptFileContent, std::deque<ska::bytecode::Instruction>& instructions) {
	auto instructionsLinesSerialized = std::vector<std::vector<std::string>>{};
	auto tokenLinesSerialized = std::vector<std::stringstream>{};
	instructionsLinesSerialized.emplace_back();
	tokenLinesSerialized.emplace_back();

	auto index = std::size_t{ 0 };
	for (const auto& token : scriptFileContent) {
		if (token != '\n') {
			tokenLinesSerialized.back() << token;
		}
		PrintInstructions(instructionsLinesSerialized.back(), index, instructions);
		if (token == '\n') {
			EndTokenAndInstructionLines(tokenLinesSerialized, instructionsLinesSerialized);
		}
		index++;
	}

	PrintTokenAndInstructionLinesInTwoColumns(stream, fixedWidth, std::move(tokenLinesSerialized), std::move(instructionsLinesSerialized));

	assert(instructions.empty());
}

void ska::bytecode::InstructionsDebugInfo::print(std::ostream& stream, ScriptCache& cache, std::size_t id) const {
	if (m_scriptFileContent.empty()) {
		return;
	}
	auto instructions = std::deque<Instruction>{ cache[id].begin(), cache[id].end() };

	PrintDequeInstructions(stream, m_columnWidth, m_scriptFileContent, instructions);
}

void ska::bytecode::InstructionsDebugInfo::print(std::ostream& stream, const InstructionOutput& generation) const {
	if (m_scriptFileContent.empty()) {
		return;
	}
	auto instructions = std::deque<Instruction> { generation.begin(), generation.end() };

	PrintDequeInstructions(stream, m_columnWidth, m_scriptFileContent, instructions);
}
