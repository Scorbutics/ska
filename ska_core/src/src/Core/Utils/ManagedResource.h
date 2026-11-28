#pragma once

#include <memory>
#include <unordered_map>

namespace ska {
	/**
	 * \brief Base class of memory automatically managed resources
	 * \tparam Value 
	 * \tparam Key 
	 */
	template <class Value, class Key>
	class ManagedResource {

	public:
		void loadFromKey(const Key& key) {
			m_key = key;
			if (m_container.find(m_key) == m_container.end() || m_container[m_key].lock() == nullptr) {
				lifetimeSeparation();
				m_container[m_key] = m_value;
			} else {
				m_value = m_container[m_key].lock();
			}
		}

		// A resource built by the caller (not from its key), shared under the key : the one already there if any
		template <class Builder>
		void loadFromKey(const Key& key, Builder&& build) {
			m_key = key;
			const auto found = m_container.find(m_key);
			m_value = found == m_container.end() ? nullptr : found->second.lock();
			if (m_value == nullptr) {
				m_value = build(m_key);
				m_container[m_key] = m_value;
			}
		}

		void lifetimeSeparation() {
			m_value = std::make_shared<Value>(m_key);
		}

		virtual ~ManagedResource() {
			free(); 
		}

		void operator=(const ManagedResource<Value, Key>& text) {
			m_key = std::move(text.m_key);
			m_value = text.m_value;
		}

	protected:
		ManagedResource() {
			free(); 
		}

		explicit ManagedResource(const Key& key) {
			free();
			ManagedResource<Value, Key>::loadFromKey(key);
		}
		
		static void freeAll() {
			m_container.clear();
		}

	private:
		void free() { m_value = nullptr; }
		static std::unordered_map<Key, std::weak_ptr<Value>> m_container;
		Key m_key;

	protected:
		std::shared_ptr<Value> m_value;
	
	};

	template<class Value, class Key>
	std::unordered_map<Key, std::weak_ptr<Value>> ManagedResource<Value, Key>::m_container;
}

