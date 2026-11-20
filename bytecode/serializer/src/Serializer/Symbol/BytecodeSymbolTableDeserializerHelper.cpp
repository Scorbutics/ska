#include <algorithm>
#include "Config/LoggerConfigLang.h"
#include "BytecodeSymbolTableDeserializerHelper.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Base/Serialization/SerializerSafeZone.h"
#include "Serializer/BytecodeOperandSerializer.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptHandleAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SymbolTableDeserializerHelper);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableDeserializerHelper)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableDeserializerHelper)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::SymbolTableDeserializerHelper)

ska::bytecode::SymbolTableDeserializerHelper::SymbolTableDeserializerHelper(ScriptCache& cache) :
	m_cache(&cache) {
}

ska::bytecode::TreeMapSymbolTableBuilder& ska::bytecode::SymbolTableDeserializerHelper::getSymbolTableBuilder(const std::string& scriptName) {
	if (m_symbolTableBuilder[scriptName] == nullptr) {
		// Virtually rebuild a ScriptAST to refer its symbol table
		if (m_cache->astCache.atOrNull(scriptName) == nullptr) {
			auto scriptAST = ScriptAST{ m_cache->astCache, scriptName, {} };
			LOG_INFO << "Script AST \"" << scriptName << "\" was empty-built for deserialization of its symbol table";
		}
		m_symbolTableBuilder[scriptName] = std::make_unique<TreeMapSymbolTableBuilder>(m_cache->astCache.at(scriptName).symbols());
	}

	return *m_symbolTableBuilder[scriptName];
}

ska::ScopedSymbolTable& ska::bytecode::SymbolTableDeserializerHelper::buildSymbol(detail::SerializerSafeZone& zone, const std::string& absoluteScriptKey, std::string symbolName, const Operand& operand, bool exported) {
	ScopedSymbolTable* symbolTable = nullptr;
	std::size_t scriptId;

	if (!absoluteScriptKey.empty()) {
		const auto scriptDelimiter = absoluteScriptKey.find_first_of('.');
		std::size_t scriptNativeStrId;
		if (scriptDelimiter != std::string::npos) {
			scriptNativeStrId = std::atoi(absoluteScriptKey.substr(0, scriptDelimiter).c_str());
		} else {
			scriptNativeStrId = std::atoi(absoluteScriptKey.c_str());
		}
		const auto& scriptName = zone.ref(scriptNativeStrId);
		symbolTable = getSymbolTableBuilder(scriptName).value(absoluteScriptKey, symbolName, exported);
		scriptId = m_cache->id(scriptName);
	} else {
		LOG_ERROR << "Empty symbol script key provided";
	}

	if (symbolTable == nullptr || symbolTable->symbol() == nullptr) {
		auto ss = std::stringstream{};
		ss << "bad symbol key provided \"" << absoluteScriptKey << "\"";
		throw std::runtime_error(ss.str());
	}

	LOG_DEBUG << "Got symbol \"" << symbolTable->name() << "\" from absolute script key \"" << absoluteScriptKey << "\"";

	m_cache->at(scriptId).helper().declareSymbol(*symbolTable->symbol(), operand);
	LOG_INFO << "Declared symbol as VAR \"" << symbolTable->name() << "\" with operand \"" << operand << "\"";

	const auto* oldSymbolInfo = m_cache->getSymbolInfo(*symbolTable->symbol());
	auto scopeIndex = std::count(absoluteScriptKey.begin(), absoluteScriptKey.end(), '.');
	scopeIndex = scopeIndex == 0 ? 0 : (scopeIndex - 1);
	auto symbolInfo = SymbolInfo{ static_cast<std::size_t>(scopeIndex + 1), symbolName, scriptId };

	if (oldSymbolInfo != nullptr) {
		LOG_INFO << "Symbol \"" << symbolTable->name() << "\" already has symbol info (it should be a binded symbol)";
		symbolInfo.binding = oldSymbolInfo->binding;
		symbolInfo.bindingPassThrough = oldSymbolInfo->bindingPassThrough;
	}

	const auto childIndexDelimiter = absoluteScriptKey.find_last_of('.');
	if (childIndexDelimiter != std::string::npos) {
		symbolInfo.childIndex = std::atoi(absoluteScriptKey.substr(childIndexDelimiter + 1).c_str());
	} else {
		symbolInfo.childIndex = 0;
	}

	symbolInfo.exported = scopeIndex == 0;

	LOG_INFO << "%12cRegistering symbol info " << symbolInfo << " for symbol " << *symbolTable << (symbolInfo.exported ? "(exported symbol)" : " (this symbol is not exported)");

	m_cache->setSymbolInfo(*symbolTable->symbol(), std::move(symbolInfo));

	return *symbolTable;
}

ska::bytecode::Operand ska::bytecode::SymbolTableDeserializerHelper::readOperand(SerializerSafeZone<17> safeZone) {
	return OperandSerializer::read(*m_cache, std::move(safeZone));
}


void ska::bytecode::SymbolTableDeserializerHelper::deferImplement(ScopedSymbolTable& classTable, ScopedSymbolTable& instanceTable) {
	m_deferredImplements.emplace_back(&classTable, &instanceTable);
}

void ska::bytecode::SymbolTableDeserializerHelper::applyDeferredImplements() {
	for (auto& [classTable, instanceTable] : m_deferredImplements) {
		classTable->implement(*instanceTable);
	}
	m_deferredImplements.clear();

	// Symbols may have been created out of order (forward references) : restore the exported fields order by position
	for (auto& [scriptName, builder] : m_symbolTableBuilder) {
		if (builder != nullptr) {
			builder->root().rebuildExportsOrder();
		}
	}
}
