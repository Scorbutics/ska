#pragma once

#include <cassert>
#include <ostream>
#include <vector>
#include "NodeValue.h"

namespace ska {
	class NodeValue;

	class PlainMemoryTable {
	public:
		const NodeValue& operator[](std::size_t index) const {
			assert(m_data.size() > index);
			return m_data[index];
		}

		const NodeValue* get_if(std::size_t index) const {
			if (index < m_data.size()) {
				return &m_data[index];
			}
			return nullptr;
		}

		// Stores "src" at "index" without any write-through : the previous value (even a cell reference) is replaced
		template <class T>
		void replace(std::size_t index, T&& src) {
			if (index >= m_data.size()) {
				m_data.resize(index + 1);
			}
			m_data[index].~NodeValue();
			new (&m_data[index]) NodeValue(std::forward<T>(src));
		}

		// Raw access, without resolving cell references
		const NodeValue& raw(std::size_t index) const {
			assert(m_data.size() > index);
			return m_data[index];
		}

		template <class T>
		void push(std::size_t index, T&& src) {
			if(index >= m_data.size()) {
				if(index == m_data.size()) {
					m_data.push_back(std::forward<T>(src));
					return;
				}
				m_data.resize(index + 1);
			}
			m_data[index] = std::forward<T>(src);
		}

		const auto& back() const { return m_data.back(); }

		bool empty() const { return m_data.empty(); }
		auto size() const { return m_data.size(); }

		friend std::ostream& operator<<(std::ostream& stream, const PlainMemoryTable&);
	private:
		std::vector<NodeValue> m_data;
	};

	std::ostream& operator<<(std::ostream& stream, const PlainMemoryTable& table);

}
