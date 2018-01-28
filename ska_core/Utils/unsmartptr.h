#pragma once
#include <memory>

namespace ska {
	namespace meta {

		template <class C>
		struct unsmart_ptr;

		template <class C>
		struct unsmart_ptr<std::unique_ptr<C>> {
			using type = C;
		};

		template <class C>
		struct unsmart_ptr<std::unique_ptr<C>&> {
			using type = C;
		};

		template <class C>
		struct unsmart_ptr<std::shared_ptr<C>> {
			using type = C;
		};

		template <class C>
		struct unsmart_ptr<std::shared_ptr<C>&> {
			using type = C;
		};
	}
}