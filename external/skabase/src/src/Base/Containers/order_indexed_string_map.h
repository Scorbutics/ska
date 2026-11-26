#pragma once
#include "insertion_indexed_map.h"
#include "Base/Meta/IsSmartPtr.h"
namespace ska {
	template<class T>
	using order_indexed_string_map_parent = insertion_indexed_map<std::string, std::conditional_t<is_smart_ptr<T>::value, T, std::unique_ptr<T>>>;

	template <class T>
	class order_indexed_string_map :
		public order_indexed_string_map_parent<T> {
	public:
		auto& emplaceNamed(T element, bool force = false) {
			auto key = element.name();
			order_indexed_string_map_parent<T>::emplace(key, std::move(element), force);
			return order_indexed_string_map_parent<T>::at(key);
		}
	};
}
