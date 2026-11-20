#include <algorithm>
#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGeneration.h"
#include "Generator/BytecodeGenerator.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ScriptGeneration);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ScriptGeneration)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::ScriptGeneration)
#define LOG_WARN SLOG_STATIC(ska::LogLevel::Warn, ska::bytecode::ScriptGeneration)

static ska::bytecode::ScriptGenerationHelper AddScript(ska::bytecode::ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) {
	auto ast = ska::ScriptAST{ cache.astCache, name, std::move(tokens) };
	return { cache, ast };
}

ska::bytecode::ScriptGeneration::ScriptGeneration(ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) :
	ScriptGeneration(std::move(AddScript(cache, std::move(tokens), name))) {
}

ska::bytecode::ScriptGeneration::ScriptGeneration(ScriptGenerationHelper origin, InstructionOutput instruction, ExportSymbolContainer symbols) :
	m_origin(std::move(origin)),
	m_generated(std::move(instruction)),
	m_exports(std::move(symbols))
#ifndef NDEBUG
	, m_debugInfo(*this)
#endif
	{
}

const ska::ASTNode& ska::bytecode::ScriptGeneration::rootASTNode() const {
	return m_origin.program().rootNode();
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGeneration& output) {
	stream << output.m_origin.program();
	stream << std::endl;
	stream << output.m_generated;
	return stream;
}

void ska::bytecode::ScriptGeneration::generate(ScriptCache& cache, Generator& generator) {
	if (m_generated.empty()) {
		auto context = GenerationContext{ cache, ScriptGenerationHelper{cache, m_origin.program()} };
		m_generated = generator.generatePart(context);
	} else {
		LOG_WARN << "Script already generated";
	}
	cache.getExportedSymbols(id());
}

std::optional<ska::bytecode::Operand> ska::bytecode::ScriptGeneration::getSymbol(const Symbol& symbol) const {
	return m_origin.getSymbol(symbol);
}

void ska::bytecode::ScriptGeneration::generate(ScriptCache& cache, InstructionOutput instructions) {
	if (m_generated.empty()) {
		m_generated = std::move(instructions);
	} else {
		LOG_WARN << "Script already generated";
	}
	cache.getExportedSymbols(id());
}


bool ska::bytecode::operator==(const ScriptGeneration& left, const ScriptGeneration& right) {
	// Exports are compared by value, in order : their keys are symbol addresses, which differ between
	// a generated script and the same script rebuilt from its serialized form.
	const auto exportsEqual = left.m_exports.size() == right.m_exports.size() &&
		std::equal(left.m_exports.begin(), left.m_exports.end(), right.m_exports.begin());
	return exportsEqual && left.m_generated == right.m_generated;
}

bool ska::bytecode::operator!=(const ScriptGeneration& left, const ScriptGeneration& right) {
	return !operator==(left, right);
}
