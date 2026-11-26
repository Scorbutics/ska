#pragma once

#include <unordered_map>
#include <tuple>
#include <memory>
#include <optional>
#include "BytecodeOperand.h"

namespace ska {
	class Symbol;
	namespace bytecode {
		class UniqueSymbolGetterBase {
			//MSVC 2017 does not like noexcept move constructor with unordered_map contained,
			//because it thinks unordered_map move constructor is not noexcept...
			//so we wrap the map into a std::unique_ptr as a workaround
			using SymbolUIDContainer = std::unique_ptr<std::unordered_map<const Symbol*, std::size_t>>;
		public:
			virtual ~UniqueSymbolGetterBase() = default;

		protected:
			UniqueSymbolGetterBase(char symbol) : m_symbol(symbol), m_container(std::make_unique<std::unordered_map<const Symbol*, std::size_t>>()) {}

			UniqueSymbolGetterBase(UniqueSymbolGetterBase&&) noexcept = default;
			UniqueSymbolGetterBase& operator=(UniqueSymbolGetterBase&&) noexcept = default;
			UniqueSymbolGetterBase& operator=(const UniqueSymbolGetterBase&) = delete;
			UniqueSymbolGetterBase(const UniqueSymbolGetterBase&) = delete;

			OperandUse query(std::size_t script, const ASTNode& node);
			OperandUse query(std::size_t script, const Symbol& symbol, Cursor positionInScript = {});
			std::optional<Operand> get(std::size_t script, const Symbol& symbol) const;
			void declare(std::size_t script, const Symbol& symbol, Operand operand);

			SymbolUIDContainer m_container;
			std::size_t m_count = 0;
			char m_symbol = '_';
		};

		template <char SymbolChar>
		class UniqueSymbolGetter :
			public UniqueSymbolGetterBase {

		public:
			UniqueSymbolGetter() : UniqueSymbolGetterBase(SymbolChar) {}
			UniqueSymbolGetter(UniqueSymbolGetter&&) noexcept = default;
			UniqueSymbolGetter& operator=(UniqueSymbolGetter&&) noexcept = default;
			UniqueSymbolGetter& operator=(const UniqueSymbolGetter&) = delete;
			UniqueSymbolGetter(const UniqueSymbolGetter&) = delete;
			~UniqueSymbolGetter() override = default;
		};
  }
}
