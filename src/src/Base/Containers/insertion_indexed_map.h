#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>

#include "Base/Meta/IsSmartPtr.h"

namespace ska {

	template <class Key, class ValueRaw>
	struct insertion_indexed_map {
	private:
		static constexpr auto IsPtrValue = is_smart_ptr<ValueRaw>::value || std::is_pointer_v<ValueRaw>;
		using Value = std::conditional_t<IsPtrValue, ValueRaw, std::optional<ValueRaw>>;
		using ValueUnderlyingType = std::conditional_t<IsPtrValue, typename remove_smart_ptr<typename std::remove_pointer_t<ValueRaw>>::type, ValueRaw>;
		using ValueLinearContainer = std::vector<Value>;
		using KeyContainer = std::unordered_map<Key, std::size_t>;
	public:
		auto find(const Key& key) const {
			auto itName = m_keys.find(key);
			if (itName == m_keys.end()) { return m_values.end(); }
			auto it = m_values.begin();
			std::advance(it, itName->second);
			return it;
		}

		auto find(const Key& key) {
			auto itName = m_keys.find(key);
			if (itName == m_keys.end()) { return m_values.end(); }
			auto it = m_values.begin();
			std::advance(it, itName->second);
			return it;
		}

		auto& operator[](std::size_t index) {
			assert(index < m_values.size());
			return *m_values[index];
		}

		const auto& operator[](std::size_t index) const {
			assert(index < m_values.size());
			return *m_values[index];
		}

		auto begin() { return m_values.begin(); }
		auto end() { return m_values.end(); }

		auto begin() const { return m_values.begin(); }
		auto end() const { return m_values.end(); }

		auto size() const { return m_values.size(); }
		auto empty() const { return m_values.empty(); }
		auto& back() { if(m_values.back() == nullptr) { throw std::runtime_error("null back element"); } return *m_values.back(); }
		const auto& back() const { if(m_values.back() == nullptr) { throw std::runtime_error("null back element"); } return *m_values.back(); }

		bool exist(std::size_t index) const {
			if (index >= m_values.size()) {
				return false;
			}

			return valueExists(m_values[index]);
		}

		auto* atOrNull(const Key& key) {
			const auto it = m_keys.find(key);
			return it != m_keys.end() ? atOrNull(it->second) : nullptr;
		}

		const auto* atOrNull(const Key& key) const {
			const auto it = m_keys.find(key);
			return it != m_keys.end() ? atOrNull(it->second) : nullptr;
		}

		ValueUnderlyingType& at(const Key& key) {
			const auto index = m_keys.at(key);
			auto* result = atOrNull(index);
			if (result == nullptr) {
				auto ss = std::stringstream{};
				ss << "bad element key \"" << key << "\" with index \"" << index << "\"";
				throw std::runtime_error(ss.str());
			}
			return *result;
		}

		const ValueUnderlyingType& at(const Key& key) const {
			const auto index = m_keys.at(key);
			auto* result = atOrNull(index);
			if (result == nullptr) {
				auto ss = std::stringstream{};
				ss << "bad element key \"" << key << "\" with index \"" << index << "\"";
				throw std::runtime_error(ss.str());
			}
			return *result;
		}

		ValueUnderlyingType& at(std::size_t index) {
			if (!exist(index)) {
				throw std::runtime_error("bad element index \"" + std::to_string(index) + "\"");
			}
			return *m_values[index];
		}

		const ValueUnderlyingType& at(std::size_t index) const {
			if (!exist(index)) {
				throw std::runtime_error("bad element index \"" + std::to_string(index) + "\"");
			}
			return *m_values[index];
		}

		template <class ValueLocal>
		bool emplace(Key key, ValueLocal&& element, bool force = false) {
			const auto wantedElementId = findNextAvailableElementId();
			const auto emplacedItem = m_keys.emplace(std::move(key), wantedElementId);
			if(emplacedItem.second) {
				pushCache(wantedElementId, std::forward<ValueLocal>(element));
				return false;
			} else if (force || !exist(emplacedItem.first->second)) {
				pushCache(emplacedItem.first->second, std::forward<ValueLocal>(element));
			}
			// Key already present and not forced : keep the existing element untouched.
			// (Do not erase the key here : it would orphan the existing element, and "key" may have been moved-from anyway)
			return true;
		}

		void set(std::size_t index, Value element) {
			if (index >= m_values.size()) {
				throw std::runtime_error("unable to add a new element by using the index setter : use \"emplace\" first.");
			}
			if (!valueExists(element)) {
				throw std::runtime_error("unable to set an empty element by using the index setter.");
			}
			m_values[index] = std::move(element);
		}

		std::size_t id(const Key& key) {
			if(m_keys.find(key) == m_keys.end()) {
				const auto wantedId = findNextAvailableElementId();
				m_keys.emplace(key, wantedId);
				pushCache(wantedId, Value{});
				return wantedId;
			}
			return m_keys.at(key);
		}

		std::size_t id(const Key& key) const {
			return m_keys.at(key);
		}

		void resizeIfTooSmall(std::size_t length) {
			if (m_values.size() < length) {
				m_values.resize(length);
			}
		}

		void clear() {
			m_keys.clear();
			m_values.clear();
		}

		const Key* findKey(std::size_t index) const {
			for (const auto& [key, index_] : m_keys) {
				if (index_ == index) {
					return &key;
				}
			}
			return nullptr;
		}

		template <class Key_, class Value_>
		bool operator==(const insertion_indexed_map<Key_, Value_>& right) const {
			return m_values == right.m_values && m_keys == right.m_keys;
		}

		template <class Key_, class Value_>
		bool operator!=(const insertion_indexed_map<Key_, Value_>& right) const {
			return !operator==(right);
		}

	private:
		static bool valueExists(const Value& value) {
			if constexpr (IsPtrValue) {
				return value != nullptr;
			} else {
				return value.has_value();
			}
		}

		std::size_t findNextAvailableElementId() const {
			return m_keys.size();
		}

		ValueUnderlyingType* atOrNull(std::size_t index) {
			if (!exist(index)) {
				return nullptr;
			}
			if constexpr (IsPtrValue) {
				if constexpr(is_smart_ptr<ValueRaw>::value) {
					return m_values[index].get();
				} else {
					return m_values[index];
				}
			} else {
				return &m_values[index].value();
			}
		}

		const ValueUnderlyingType* atOrNull(std::size_t index) const {
			if (!exist(index)) {
				return nullptr;
			}
			if constexpr (IsPtrValue) {
				if constexpr(is_smart_ptr<ValueRaw>::value) {
					return m_values[index].get();
				} else {
					return m_values[index];
				}
			} else {
				return &m_values[index].value();
			}
		}

		template <class ValueLocal>
		void pushCache(std::size_t index, ValueLocal&& element) {
			resizeIfTooSmall(index + 1);
			if constexpr (is_smart_ptr<ValueLocal>::value || std::is_pointer_v<ValueLocal> || !IsPtrValue) {
				m_values[index] = std::forward<ValueLocal>(element);
			} else {
				m_values[index] = ValueRaw{ new ValueUnderlyingType(std::forward<ValueLocal>(element)) };
			}
		}

		KeyContainer m_keys;
		ValueLinearContainer m_values;
	};

}
