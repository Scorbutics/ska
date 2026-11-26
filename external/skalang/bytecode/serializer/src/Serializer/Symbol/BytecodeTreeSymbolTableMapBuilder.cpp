#include "BytecodeTreeSymbolTableMapBuilder.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"
#include "Base/Values/Strings/StringUtils.h"

ska::bytecode::TreeSymbolTableMapBuilder::TreeSymbolTableMapBuilder(const SymbolTable& table) {
	store(table, "");
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const SymbolTable& table, const std::string& defaultDepth) {
	store(table.root(), defaultDepth);
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const ScopedSymbolTable& table, const std::string& depth) {
	// Each child symbol table (named = symbol, or unnamed = block scope) is keyed by its position among all children of its parent.
	// This is the layout expected by TreeMapSymbolTableBuilder when rebuilding the tree (see walkScope).
	auto position = std::size_t {0};
	for (const auto& childTable : table) {
		if (childTable == nullptr) {
			throw std::runtime_error("invalid child symbol table (index " + std::to_string(position) + ") of parent \"" + table.name() + "\"");
		}

		if (childTable->symbol() != nullptr) {
			// Store child table direct data (symbol)
			store(*childTable, depth, position);
		}

		// Store child table children data, nested under the child table key
		store(*childTable, buildKey(depth, position));
		position++;
	}
}

bool ska::bytecode::SymbolKeyCompare::operator()(const std::string& lhs, const std::string& rhs) const {
	auto lhsIt = std::size_t{ 0 };
	auto rhsIt = std::size_t{ 0 };
	while (lhsIt < lhs.size() && rhsIt < rhs.size()) {
		const auto lhsEnd = std::min(lhs.find('.', lhsIt), lhs.size());
		const auto rhsEnd = std::min(rhs.find('.', rhsIt), rhs.size());
		const auto lhsPart = std::stoull(lhs.substr(lhsIt, lhsEnd - lhsIt));
		const auto rhsPart = std::stoull(rhs.substr(rhsIt, rhsEnd - rhsIt));
		if (lhsPart != rhsPart) {
			return lhsPart < rhsPart;
		}
		lhsIt = lhsEnd + 1;
		rhsIt = rhsEnd + 1;
	}
	// A prefix (parent) comes before its children
	return lhsIt < rhsIt ? false : (lhsIt >= lhs.size() && rhsIt < rhs.size());
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const ScopedSymbolTable& symbol, const std::string& depth, std::size_t childIndex) {
	const auto existingSymbolIt = m_symbols.find(&symbol);
	if (existingSymbolIt == m_symbols.end()) {
		auto key = buildKey(depth, childIndex);
		m_symbols.emplace(&symbol, key);
		m_symbolsReversedWrite.emplace(std::move(key), &symbol);
	}
}

std::string ska::bytecode::TreeSymbolTableMapBuilder::buildKey(const std::string& depth, std::size_t childIndex) {
	auto strChildIndex = std::to_string(childIndex);
	return depth.empty() ? strChildIndex : depth + "." + strChildIndex;
}

std::string ska::bytecode::TreeSymbolTableMapBuilder::key(const ScopedSymbolTable& symbol) const {
	auto symbolSeekIt = m_symbols.find(&symbol);
	return symbolSeekIt == m_symbols.end() ? "" : symbolSeekIt->second;
}

void ska::bytecode::TreeSymbolTableMapBuilder::write(SerializerOutput& output, SymbolTableSerializer& serializer) const {
	serializer.writeFull(output, m_symbolsReversedWrite);
}
