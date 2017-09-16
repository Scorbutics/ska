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
				m_value = std::make_shared<Value>(m_key);
				m_container[m_key] = m_value;
			} else {
				m_value = m_container[m_key].lock();
			}
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

