#include "Config/LoggerConfigLang.h"
#include "UniqueSymbolGetter.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/Symbol.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::UniqueSymbolGetterBase);

ska::bytecode::OperandUse ska::bytecode::UniqueSymbolGetterBase::query(std::size_t script, const ASTNode& node) {
	if (node.symbol() == nullptr) {
		SLOG(ska::LogLevel::Debug) << "Querying symbol node with value \"" << node.name() << "\" (no symbol)";
		return OperandUse { node };
	}

	return query(script, *node.symbol(), node.positionInScript());
}

ska::bytecode::OperandUse ska::bytecode::UniqueSymbolGetterBase::query(std::size_t script, const Symbol& symbol, Cursor positionInScript) {
	bool isNew = false;
	auto varCount = m_container->find(&symbol);
	if (varCount == m_container->end()) {
		varCount = m_container->emplace(&symbol, m_count++).first;
		isNew = true;
	}
	auto ss = std::stringstream{};
	ss << m_symbol << varCount->second;

	SLOG(ska::LogLevel::Debug) << "Querying symbol node \"" << symbol.name() << "\" with value " << ss.str() << (isNew ? " (new)" : "") << " address " << &symbol;

	auto result = OperandUse{ ScriptVariableRef{ varCount->second, script }, OperandType::VAR, std::move(positionInScript) };
	result.isFirstTimeUsed = isNew;
	return result;
}

std::optional<ska::bytecode::Operand> ska::bytecode::UniqueSymbolGetterBase::get(std::size_t script, const Symbol& symbol) const {
	auto varCount = m_container->find(&symbol);
	if (varCount == m_container->end()) {
		SLOG(ska::LogLevel::Debug) << "No symbol \"" << symbol.name() << "\" found in script id " << script << ", returning new operand";
		return {};
	}
	return Operand{ ScriptVariableRef{ varCount->second, script }, OperandType::VAR };
}

void ska::bytecode::UniqueSymbolGetterBase::declare(std::size_t script, const Symbol& symbol, Operand operand) {
	auto varCount = m_container->find(&symbol);

	if (!std::holds_alternative<ScriptVariableRef>(operand.content())) {
		auto ss = std::stringstream{};
		ss << "symbol \"" << symbol.name() << "\" is not a variable";
		throw std::runtime_error(ss.str());
	}

	const auto wantedSymbolId = operand.as<ScriptVariableRef>().variable;
	if (varCount == m_container->end()) {
		varCount = m_container->emplace(&symbol, wantedSymbolId).first;
	} else if (varCount->second != wantedSymbolId) {
		auto ss = std::stringstream{};
		ss << "already existing symbol id for the symbol \"" << symbol.name() << "\"";
		throw std::runtime_error(ss.str());
	}

	m_count = wantedSymbolId >= m_count ? (wantedSymbolId + 1) : m_count;
}
