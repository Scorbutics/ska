#pragma once
#include <unordered_map>
#include <iterator>
#include <vector>
#include "Base/Meta/IsSmartPtr.h"
namespace ska {

  template <template <class T> class Container, class Key, class Value>
  class container_mapped {
  using ContainerRef = typename std::conditional_t<
    std::is_pointer_v<Container<Value>> || ska::is_smart_ptr<Container<Value>>::value,
    std::remove_pointer_t<typename ska::remove_smart_ptr<Container<Value>>::type>&,
    Container<Value>&
  >;

  public:
    container_mapped(Container<Value>&& origin) :
    m_container(std::forward<Container<Value>>(origin)),
    m_containerRef(getContainerRef()) {}

    container_mapped() :
      m_containerRef(getContainerRef()) {}

    ~container_mapped() = default;

    ContainerRef& getContainerRef() {
      if constexpr (std::is_pointer_v<Container<Value>> || ska::is_smart_ptr<Container<Value>>::value) {
        return *m_container;
      } else {
        return m_container;
      }
    }

    template <class KeyT, class ValueT>
    void emplace(KeyT&& key, ValueT&& value) {
      auto index = m_map.size();
      const auto [insertedIt, insertionHappened] = m_map.emplace(std::forward<KeyT>(key), index);
      index = std::distance(m_map.begin(), insertedIt);

      if (index >= m_containerRef.size()) {
        if (index == m_containerRef.size() + 1) {
          m_containerRef.push_back(std::forward<Value>(value));
          return;
        } else {
          m_containerRef.resize(index + 1);
        }
      }
      m_containerRef[index] = std::forward<Value>(value);
    }

    Value& operator[](const Key& key) {
      const auto index = m_map.at(key);
      assert(index < m_containerRef.size());
      return m_containerRef[index];
    }

    const Value& operator[](const Key& key) const {
      const auto index = m_map.at(key);
      assert(index < m_containerRef.size());
      return m_containerRef[index];
    }

    const Container<Value>& underlying() const {
      return m_container;
    }

  private:
    std::unordered_map<Key, std::size_t> m_map;
    Container<Value> m_container;
    ContainerRef m_containerRef;
  };

}