#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptCache.h"

#include "NodeValue/AST.h"
#include "NodeValue/Symbol.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ScriptCache);


void ska::bytecode::ScriptCache::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	if (node.symbol() == nullptr) {
		throw std::runtime_error("Cannot set symbol information for a node without symbol : " + node.name());
	}
	return setSymbolInfo(*node.symbol(), std::move(info));
}

void ska::bytecode::ScriptCache::setSymbolInfo(const Symbol& symbol, SymbolInfo info) {
	SLOG(ska::LogLevel::Debug) << "[Cache " << this << "] Setting " << info << " (" << symbol.name() << ") with key 0x" << &symbol;
	auto it = m_symbolInfo.find(&symbol);
	if (it == m_symbolInfo.end()) {
		m_symbolInfo.emplace(&symbol, std::move(info));
	} else {
		it->second = std::move(info);
	}
}

ska::bytecode::SymbolInfo ska::bytecode::ScriptCache::getSymbolInfoOrNew(std::size_t scriptIndex, const Symbol& symbol) const {
	const auto* result = getSymbolInfo(symbol);
	return result == nullptr ? SymbolInfo {scriptIndex} : *result;
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptCache::getSymbolInfo(const Symbol& symbol) const {
	SLOG(ska::LogLevel::Debug) << "[Cache " << this << "] Getting " << symbol.name() << " with key 0x" << &symbol;
	if(m_symbolInfo.find(&symbol) == m_symbolInfo.end()) {
		SLOG(ska::LogLevel::Debug) << "\t NULL";
		return nullptr;
	}
	SLOG(ska::LogLevel::Debug) << "\t NOT NULL";
	return &m_symbolInfo.at(&symbol);
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptCache::getSymbolInfo(const ASTNode& node) const {
	if(node.symbol() == nullptr) {
		return nullptr;
	}
	return getSymbolInfo(*node.symbol());
}

const ska::bytecode::ExportSymbolContainer& ska::bytecode::ScriptCache::getExportedSymbols(std::size_t scriptIndex) {
	// TODO : ce n'est pas à un "get" de faire tout ça...
	if ((*this)[scriptIndex].exportedSymbols().empty()) {
		SLOG(ska::LogLevel::Info) << "%11cGenerating exported symbols for script \"" << scriptIndex << "\"";
		auto temporarySortedScriptSymbols = std::priority_queue<SymbolWithInfo>{};
		for (const auto& data : m_symbolInfo) {
			if (data.second.exported && data.second.script == scriptIndex) {
				SLOG(ska::LogLevel::Info) << "%11c\tFound symbol " << data.first->name() << " with info " << data.second;
				temporarySortedScriptSymbols.push(SymbolWithInfo{ data.first, &data.second });
			}
		}

		if (!temporarySortedScriptSymbols.empty()) {
			auto exports = (*this)[scriptIndex].generateExportedSymbols(std::move(temporarySortedScriptSymbols));
			if (exports.has_value()) {
				(*this)[scriptIndex].setExportedSymbols(std::move(exports.value()));
			} else {
				SLOG(ska::LogLevel::Error) << "%11cExported symbols already set for script \"" << scriptIndex << "\" required";
			}
		}
	} else {
		SLOG(ska::LogLevel::Info) << "%11cNo generation of exported symbols for script \"" << scriptIndex << "\" required";
	}
	return (*this)[scriptIndex].exportedSymbols();
}

bool ska::bytecode::ScriptCache::isGenerated(std::size_t index) const {
	return index < size() && exist(index) && !(*this)[index].empty();
}

void ska::bytecode::ScriptCache::storeBinding(NativeFunctionPtr binding, const ScriptVariableRef& bindingRef) {
	auto finalId = std::to_string(bindingRef.variable) + " " + at(bindingRef.script).name();
	m_bindings.emplace(std::move(finalId), std::move(binding), true);
}

const ska::NativeFunction& ska::bytecode::ScriptCache::getBinding(const ScriptVariableRef& bindingRef) const {
	auto finalId = std::to_string(bindingRef.variable) + " " + at(bindingRef.script).name();
	auto binding = m_bindings.atOrNull(finalId);
	if (binding == nullptr) {
		auto ss = std::stringstream {};
		ss << "unable to find binding with id \"" << finalId << "\"";
		throw std::runtime_error(ss.str());
	}
	return *binding;
}

void ska::bytecode::ScriptCache::printDebugInfo(std::ostream& stream) const {
#ifndef NDEBUG
	for (const auto& script : *this) {
		stream << "Debug \"" << script->name() << std::endl << "\" info : ";
		script->printDebugInfo(stream);
	}
#endif
}
