#include "BytecodeTreeMapSymbolTableBuilder.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Generator/Value/BytecodeScriptCache.h"

ska::bytecode::TreeMapSymbolTableBuilder::TreeMapSymbolTableBuilder(SymbolTable& table) :
	m_rootRead(&table.root()) {
}

ska::ScopedSymbolTable& ska::bytecode::TreeMapSymbolTableBuilder::walkScope(const std::vector<std::string>& parts, std::string leafName, bool exported) {
	auto* currentSymbolTable = m_rootRead;

	ska::ScopedSymbolTable* parent = currentSymbolTable;
	// First part is script name native index. Last part is leaf child index.
	// avoid using both
	// Each part is the position of a child symbol table among all children of its parent (see TreeSymbolTableMapBuilder::store).
	for (std::size_t i = 1u; i < parts.size() - 1; i++) {
		const auto position = static_cast<std::size_t>(std::atoi(parts[i].c_str()));

		// Scopes without any symbol (blocks) are not serialized : rebuild them as unnamed scopes
		while (parent->size() <= position) {
			parent->createNested();
		}
		parent = (*parent)[position];
	}

	auto* symbolLeaf = (*parent)(leafName);
	if (symbolLeaf != nullptr) {
		return *symbolLeaf;
	}

	// Keep positions aligned with the serialized tree, by recreating skipped unnamed scopes before the leaf
	const auto leafPosition = static_cast<std::size_t>(std::atoi(parts.back().c_str()));
	if (leafPosition < parent->size() && (*parent)[leafPosition] != nullptr && (*parent)[leafPosition]->symbol() == nullptr) {
		// A placeholder was created at this position by a previous (forward) reference to a deeper key : name it
		return parent->nameNested(leafPosition, std::move(leafName), exported);
	}
	while (parent->size() < leafPosition) {
		parent->createNested();
	}
	return parent->createNested(std::move(leafName), nullptr, exported);
}

ska::ScopedSymbolTable* ska::bytecode::TreeMapSymbolTableBuilder::value(const std::string& key, std::optional<std::string> name, bool exported) {
	auto symbolSeekIt = m_symbolsReversedRead.find(key);

	if (symbolSeekIt == m_symbolsReversedRead.end()) {
		auto parts = StringUtils::split(key, '.');

		//parts[0] == script id => unused here because it was filtered as first
		if (parts.empty() || !name.has_value()) {
			return nullptr;
		}

		return &walkScope(std::move(parts), name.value(), exported);
	}

	return symbolSeekIt->second;
}

