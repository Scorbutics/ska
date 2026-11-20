#pragma once
#include <map>
#include <optional>
#include <unordered_map>
#include <sstream>

namespace ska {
	class SymbolTable;
	class Symbol;
	class ScopedSymbolTable;
	class SerializerOutput;

	namespace bytecode {
		class SymbolTableSerializer;

		// Orders symbol keys ("0", "0.1", "2", "10"...) numerically part by part, so that a parent scope
		// and all its previous siblings are always written (and so rebuilt) before its children.
		struct SymbolKeyCompare {
			bool operator()(const std::string& lhs, const std::string& rhs) const;
		};

		class TreeSymbolTableMapBuilder {
			using IndexSymbolMap = std::unordered_map<const ScopedSymbolTable*, std::string>;
		public:
			using ReverseIndexSymbolMapWrite = std::map<std::string, const ScopedSymbolTable*, SymbolKeyCompare>;

			TreeSymbolTableMapBuilder(const SymbolTable& table);

			TreeSymbolTableMapBuilder(TreeSymbolTableMapBuilder&&) noexcept = default;
			TreeSymbolTableMapBuilder& operator=(TreeSymbolTableMapBuilder&&) = default;

			TreeSymbolTableMapBuilder(const TreeSymbolTableMapBuilder&) = delete;
			TreeSymbolTableMapBuilder& operator=(const TreeSymbolTableMapBuilder&) = delete;

			~TreeSymbolTableMapBuilder() = default;

			std::string key(const ScopedSymbolTable& symbol) const;
			void write(SerializerOutput& output, SymbolTableSerializer& serializer) const;

		private:
			void store(const SymbolTable& table, const std::string& defaultDepth);
			void store(const ScopedSymbolTable& table, const std::string& depth);
			void store(const ScopedSymbolTable& symbol, const std::string& depth, std::size_t childIndex);

			static std::string buildKey(const std::string& depth, std::size_t childIndex);
			IndexSymbolMap m_symbols;
			ReverseIndexSymbolMapWrite m_symbolsReversedWrite;
		};
	}
}
