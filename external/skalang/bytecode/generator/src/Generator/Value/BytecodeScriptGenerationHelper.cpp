#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationHelper.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeScriptCache.h"
#include "NodeValue/ScriptHandleAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ScriptGenerationHelper);

ska::bytecode::ScriptGenerationHelper::ScriptGenerationHelper(std::size_t index, const ScriptAST& script) {
	m_script = script.handle();
	m_index = index;
}

ska::bytecode::ScriptGenerationHelper::ScriptGenerationHelper(ScriptCache& cache, const ScriptAST& script) {
	const auto& name = script.name();
	m_script = script.handle();
	m_index = cache.id(name);
}

ska::bytecode::ScriptGenerationHelper::ScriptGenerationHelper(ScriptCache& cache, StatementParser& parser, const std::string& scriptName, std::vector<Token> tokens) {
	auto scriptAst = ska::ScriptAST{ cache.astCache, scriptName, std::move(tokens) };
	scriptAst.parse(parser);
	m_script = scriptAst.handle();
	m_index = cache.id(scriptName);
}

ska::ScriptAST ska::bytecode::ScriptGenerationHelper::program() const {
	return ska::ScriptAST{ *m_script };
}

const ska::ASTNode& ska::bytecode::ScriptGenerationHelper::rootASTNode() const {
	return m_script->rootNode();
}

const std::string& ska::bytecode::ScriptGenerationHelper::name() const {
	return m_script->name();
}

ska::bytecode::Register ska::bytecode::ScriptGenerationHelper::queryNextRegister() {
	auto result = Register{ ScriptVariableRef { m_register++, m_index }, OperandType::REG };
	result.isFirstTimeUsed = true;
	return result;
}

ska::bytecode::OperandUse ska::bytecode::ScriptGenerationHelper::querySymbolOrOperand(const ASTNode& node) {
	return VariableGetter::query(m_index, node);
}

ska::bytecode::OperandUse ska::bytecode::ScriptGenerationHelper::querySymbol(const Symbol& symbol) {
	return VariableGetter::query(m_index, symbol);
}

void ska::bytecode::ScriptGenerationHelper::declareSymbol(const Symbol& symbol, const Operand& operand) {
	VariableGetter::declare(m_index, symbol, operand);
}

std::optional<ska::bytecode::Operand> ska::bytecode::ScriptGenerationHelper::getSymbol(const Symbol& symbol) const {
	return VariableGetter::get(m_index, symbol);
}

ska::bytecode::ExportSymbolContainer ska::bytecode::ScriptGenerationHelper::generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const {
	auto result = ska::bytecode::ExportSymbolContainer{};

	while (!symbolsInfo.empty()) {
		const auto& symbolWithInfo = symbolsInfo.top();
		auto operand = VariableGetter::get(m_index, *symbolWithInfo.symbol);
		if (operand.has_value()) {
			result.emplace(symbolWithInfo.symbol, ExportSymbol{ std::move(operand.value()), symbolWithInfo.symbol });
		}
		symbolsInfo.pop();
	}

	return result;
}

ska::ScriptASTPtr ska::bytecode::ScriptGenerationHelper::useImport(const std::string& scriptImported) {
	return program().useImport(scriptImported);
}
