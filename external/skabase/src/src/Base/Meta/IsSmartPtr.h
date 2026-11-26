#pragma once
#include <memory>

namespace ska {
  template <class T>
	struct is_smart_ptr : std::false_type {};

	template <class T>
	struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

	template <class T>
	struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

	template <class T>
	struct remove_smart_ptr { using type = T; };

	template <class T>
	struct remove_smart_ptr<std::unique_ptr<T>> { using type = T; };

	template <class T>
	struct remove_smart_ptr<std::shared_ptr<T>> { using type = T; };
}
