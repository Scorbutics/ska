#pragma once

namespace ska {
	namespace dynlib {
		
		template <class Value, class ... Keys>
		struct Cache;

		template <class Value, class Key>
		struct Cache<Value, Key> {

			template <class KeyT>
			const Value* get() const {
				if constexpr (!std::is_same<KeyT, Key>::value) {
					return nullptr;
				}
				return &current;
			}
		
			template <class KeyT>
			void set(Value&& value) {
				if constexpr (std::is_same<KeyT, Key>::value) {
					current = std::forward<Value>(value);
				}
			}

			void set(const int keyId, Value&& value) {
				if (keyId == Key::id) {
					current = std::forward<Value>(value);
				}
			}
		
		private:
			Value current {};
		};
		
		template<class Value, class Key, class ... RemainingKeys>
		struct Cache<Value, Key, RemainingKeys...> {
		
			template <class KeyT>
			const Value* get() const {
				if constexpr (std::is_same<KeyT, Key>::value) {
					return &current;
				}
				return remainingCache.template get<KeyT>();
			}
		
			template <class KeyT>
			void set(Value&& value) {
				if constexpr (std::is_same<KeyT, Key>::value) {
					current = std::forward<Value>(value);
					return;
				}
				remainingCache.template set<KeyT>(std::forward<Value>(value));
			}

			void set(const int keyId , Value&& value) {
				if (keyId == Key::id) {
					current = std::forward<Value>(value);
					return;
				}
				remainingCache.set(keyId, std::forward<Value>(value));
			}
		
		private:
			Cache<Value, RemainingKeys...> remainingCache;
			Value current {};
		};
	}
}