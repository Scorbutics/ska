#pragma once
#include <optional>
#include <unordered_set>
#include "Type.h"

namespace ska {
	class Symbol;
	class ScopedSymbolTable;
	struct ScriptHandleAST;
	class SymbolFactory;
}

namespace ska {

	class Symbol {
	private:
		friend class SymbolFactory;
		Symbol() = default;

		Symbol(std::size_t tableIndex, std::string name, bool exported);

	public:
		Symbol(const Symbol& s) = delete;
		Symbol(Symbol&& s) noexcept;

		Symbol& operator=(const Symbol& s) = delete;
		Symbol& operator=(Symbol&& s) noexcept;

		const std::string& name() const { return m_name; }
		ExpressionType nativeType() const { return m_category.type(); }
		const Type& type() const { return m_category; }
		bool exported() const { return m_exported; }
		bool changeTypeIfRequired(const Type& type);

		bool operator==(const Symbol& sym) const;
		bool operator!=(const Symbol& sym) const {	return !(*this == sym);	}

	private:
		friend std::ostream& operator<<(std::ostream& stream, const Symbol& symbol);
		std::string m_name;
		std::size_t m_tableIndex = 0;
		bool m_exported = false;
		Type m_category;
		ScopedSymbolTable* m_classTable = nullptr;
		bool m_closed = true;

	};
	std::ostream& operator<<(std::ostream& stream, const Symbol& symbol);
}
