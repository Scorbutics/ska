#pragma once
#include <string>
#include <limits>

namespace ska {

	enum class SymbolTableLookupType {
		Hierarchical,
		Direct
	};

	struct SymbolTableLookup {
	private:
		SymbolTableLookup(SymbolTableLookupType type, std::string symbol) :
			type(std::move(type)),
			symbolName(std::move(symbol)) {}

	public:
		static SymbolTableLookup direct(std::string symbol) {
			return {SymbolTableLookupType::Direct, std::move(symbol) };
		}

		static SymbolTableLookup hierarchical(std::string symbol) {
			return {SymbolTableLookupType::Hierarchical, std::move(symbol) };
		}

		SymbolTableLookupType type;
		std::string symbolName;
	};

	struct SymbolTableNested {
	private:
		SymbolTableNested(int depth, std::size_t childIndex, std::string name = "") :
			depth(depth),
			childIndex(childIndex),
			childName(std::move(name)) {}

	public:
		static SymbolTableNested lastChild(std::size_t depth = 1) {
			return {static_cast<int>(depth), std::numeric_limits<std::size_t>::max() };
		}

		static SymbolTableNested firstChild(std::size_t depth = 1) {
			return {static_cast<int>(depth), 0 };
		}

		static SymbolTableNested parent(std::size_t depth = 1) {
			return {- static_cast<int>(depth), 0 };
		}

		static SymbolTableNested current() {
			return { 0, 0 };
		}

		static SymbolTableNested child(std::string name) {
			return { 0, 0, std::move(name) };
		}

		static SymbolTableNested child(std::size_t childIndex, std::size_t depth = 1) {
			return { static_cast<int>(depth), childIndex };
		}

		int depth = 0;
		std::size_t childIndex = 0;
		std::string childName;
	};
}
