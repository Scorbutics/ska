#pragma once
#include <vector>
#include <variant>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include "TokenVariant.h"
#include "NodeValueArray.h"
#include "NodeValueMap.h"

namespace ska {
	class NodeValue;
	class PlainMemoryTable;

	using NodeValueVariant_ = std::variant<
		TokenVariant,
		NodeValueArray,
		NodeValueMap
	>;

	namespace detail {
		template<typename T, typename VARIANT_T>
		struct isVariantMember;

		template<typename T, typename... ALL_T>
		struct isVariantMember<T, std::variant<ALL_T...>>
			: public std::disjunction<std::is_same<T, ALL_T>...> {};
	}

	// A runtime value.
	// It can also be a "cell reference" : a value whose storage is a shared cell (a variable captured by a closure,
	// or an object field aliasing such a variable). Reading a cell reference reads the cell, assigning it writes into the cell.
	// A function value (closure) also carries the cells it captured ("upvalues").
	class NodeValue {
	public:
		using Cell = std::shared_ptr<NodeValue>;
		using Upvalues = std::vector<Cell>;

		NodeValue():
			m_emptyVariant(true) {
		}

		template <class Arg, std::enable_if_t<
			std::is_same_v<NodeValueVariant_, std::remove_const_t<std::remove_reference_t<Arg>>> ||
			detail::isVariantMember<std::remove_const_t<std::remove_reference_t<Arg>>, NodeValueVariant_>::value ||
			detail::isVariantMember<std::remove_const_t<std::remove_reference_t<Arg>>, TokenVariant>::value, std::nullptr_t> = nullptr>
		NodeValue(Arg&& arg) :
			m_variant(std::forward<Arg>(arg)),
			m_emptyVariant(false) {
		}

		NodeValue(float f):
			NodeValue(static_cast<double>(f)) {
		}

		NodeValue(NodeValueArray arg) :
			m_variant(std::move(arg)),
			m_emptyVariant(false) {
		}

		NodeValue(NodeValueMap arg) :
			m_variant(std::move(arg)),
			m_emptyVariant(false) {
		}

		NodeValue(const NodeValue&) = default;
		NodeValue(NodeValue&&) noexcept = default;
		~NodeValue() = default;

		// Assigning a cell reference writes the (resolved) value into the cell.
		// Otherwise, the value is replaced (including by a cell reference, which then aliases the cell).
		NodeValue& operator=(const NodeValue& arg);
		NodeValue& operator=(NodeValue&& arg) noexcept;

		static NodeValue CellReference(Cell cell);
		bool isCellReference() const { return m_cell != nullptr; }
		const Cell& cell() const { return m_cell; }

		void setUpvalues(std::shared_ptr<const Upvalues> upvalues) { m_upvalues = std::move(upvalues); }
		const Upvalues* upvalues() const;

		// Refered value if this is a cell reference, otherwise this value.
		// Shallow : strings, arrays and objects are shared handles, only their reference count changes.
		NodeValue resolved() const &;
		// Same, but moves this value when it is not a cell reference (no copy at all in the common case)
		NodeValue resolved() &&;

		template<class T>
		T& as() {
			if (m_cell != nullptr) {
				return m_cell->as<T>();
			}
			return std::get<T>(m_variant);
		}

		template<class T>
		const T& as() const {
			if (m_cell != nullptr) {
				return static_cast<const NodeValue&>(*m_cell).as<T>();
			}
			return std::get<T>(m_variant);
		}

		double convertNumeric() const;
		bool empty() const { return m_cell != nullptr ? m_cell->empty() : m_emptyVariant; }
		std::string convertString() const;

		template <class Converted>
		static constexpr bool is_container_of_values() {
			return !detail::isVariantMember<Converted, TokenVariant>::value;
		}

		template <class Converted>
		Converted& nodeval() {
			return const_cast<Converted&>(static_cast<const NodeValue&>(*this).nodeval<Converted>());
		}

		template <class Converted>
		const Converted& nodeval() const {
			if (m_cell != nullptr) {
				return static_cast<const NodeValue&>(*m_cell).nodeval<Converted>();
			}
			if constexpr(detail::isVariantMember<Converted, TokenVariant>::value) {
				return std::get<Converted>(std::get<TokenVariant>(m_variant));
			} else {
				return std::get<Converted>(m_variant);
			}
		}

		friend bool operator==(const NodeValue& lhs, const NodeValue& rhs);
		friend std::ostream& operator<<(std::ostream& stream, const NodeValue&);

	private:
		NodeValueVariant_ m_variant;
		bool m_emptyVariant = false;
		// Non null when this value is a cell reference
		Cell m_cell;
		// Cells captured by a function value (closure)
		std::shared_ptr<const Upvalues> m_upvalues;
	};

	bool operator==(const NodeValue& lhs, const NodeValue& rhs);
	std::ostream& operator<<(std::ostream& stream, const NodeValue&);
}
