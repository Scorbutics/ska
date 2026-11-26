#pragma once
#include <optional>
#include <array>
#include <vector>
#include <cassert>
#include <iterator>

namespace ska {
	template <typename T, std::size_t ArraySize, class Container = std::array<std::optional<T>, ArraySize>>
	class unique_buffered_priority_indexed_fixed_queue_iterator {
		Container* buf {};
		std::size_t off;

		void safe_next_element() {
			while (off < ArraySize && !(*buf)[off].has_value()) { ++off; }
			keep_safe();
		}

		void safe_previous_element() {
			while (off > 0 && !(*buf)[off].has_value()) { --off; }
			keep_safe();
		}

		void keep_safe() {
			if ( off < 0 || off >= ArraySize || !(*buf)[off].has_value()) {
				off = ArraySize;
			}
		}

	public:
		unique_buffered_priority_indexed_fixed_queue_iterator(Container& buf, std::size_t offset, bool next = true)
			: buf(&buf), off(offset) {
			if(next) {
				safe_next_element();
			} else {
				safe_previous_element();
			}
		}
		bool operator==(const unique_buffered_priority_indexed_fixed_queue_iterator& i) {
			return i.buf == buf && i.off == off;
		}
		bool operator!=(const unique_buffered_priority_indexed_fixed_queue_iterator& i) {
			return !(*this == i);
		}
		unique_buffered_priority_indexed_fixed_queue_iterator& operator++() { off++; safe_next_element(); return *this; }
		unique_buffered_priority_indexed_fixed_queue_iterator operator++(int) { auto t = *this; off++; safe_next_element(); return t; }
		unique_buffered_priority_indexed_fixed_queue_iterator& operator--() { off--; safe_previous_element(); return *this; }
		unique_buffered_priority_indexed_fixed_queue_iterator operator--(int) { auto t = *this; off--; safe_previous_element(); return t; }
		
		unique_buffered_priority_indexed_fixed_queue_iterator operator-(int i) { return {*buf, off - i, false}; }
		unique_buffered_priority_indexed_fixed_queue_iterator operator+(int i) { return {*buf, off + i}; }

		std::size_t operator-(unique_buffered_priority_indexed_fixed_queue_iterator i) { return off - i.off; }
		std::size_t operator+(unique_buffered_priority_indexed_fixed_queue_iterator i) { return off + i.off; }

		bool operator<(unique_buffered_priority_indexed_fixed_queue_iterator const& sibling) const { return off < sibling.off; }
		bool operator<=(unique_buffered_priority_indexed_fixed_queue_iterator const& sibling) const { return off <= sibling.off; }
		bool operator>(unique_buffered_priority_indexed_fixed_queue_iterator const& sibling) const { return off > sibling.off; }
		bool operator>=(unique_buffered_priority_indexed_fixed_queue_iterator const& sibling) const { return off >= sibling.off; }
		auto& operator*() const { return (*buf)[off].value(); }
	};

	struct PropertyPriorityGetter {
		template<class T>
		std::size_t operator()(const T& value) const {
			return value.priority;
		}
	};

	template <class T, std::size_t ArraySize, class PriorityGetter = PropertyPriorityGetter>
	class unique_buffered_priority_indexed_fixed_queue {
		using BufferArrayContainer = std::array<std::vector<T>, ArraySize>;
		using ArrayContainer = std::array<std::optional<T>, ArraySize>;
	protected:
		ArrayContainer front_container;
		BufferArrayContainer buffer;
		PriorityGetter priorityGetter;
		std::size_t counter = 0;
	public:
		using iterator = unique_buffered_priority_indexed_fixed_queue_iterator<T, ArraySize>;
		using const_iterator = unique_buffered_priority_indexed_fixed_queue_iterator<T, ArraySize, const std::array<std::optional<T>, ArraySize>>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		explicit unique_buffered_priority_indexed_fixed_queue() = default;

		bool empty()       const { return size() == 0; }
		std::size_t size() const { return counter; }
		const T& top()     const { assert(counter > 0); return front_container[counter - 1].value(); }

		void push(T x) {
			const auto priority = priorityGetter(x);
			assert(priority < ArraySize && priority >= 0);
			auto & cell = front_container[priority];
			if (cell.has_value()) {
				auto newValue = std::move(cell.value());
				cell = std::move(x);
				buffer[priority].push_back(std::move(newValue));
			} else {
				cell = std::move(x);
				counter++;
			}
		}

		void pop() {
			if (remove_from_index(counter - 1)) {
				counter--;
			}
		}

		void remove(const T& x) {
			const auto priority = priorityGetter(x);
			if (priority == counter - 1) {
				pop();
			} else {
				remove_from_index(priority);
			}
		}

		void erase(iterator it) {
			remove(*it);
		}

		iterator begin() { return { front_container, 0 }; }
		iterator end() { return { front_container, ArraySize }; }
		const_iterator begin() const { return { front_container, 0 }; }
		const_iterator end() const { return { front_container, ArraySize }; }
		const_iterator cbegin() const { return begin(); }
		const_iterator cend() const { return end(); }

		reverse_iterator rbegin() { return reverse_iterator{ end() }; }
		reverse_iterator rend() { return reverse_iterator{ begin() }; }
		const_reverse_iterator rbegin() const { return const_reverse_iterator{ end() }; }
		const_reverse_iterator rend() const { return const_reverse_iterator{ begin() }; }
		const_reverse_iterator crbegin() const { return rbegin(); }
		const_reverse_iterator crend() const { return rend(); }

	private:
		bool remove_from_index(std::size_t index) {
			assert(index < ArraySize && index >= 0);
			auto & bufferPool = buffer[index];
			if (bufferPool.empty()) {
				front_container[index] = std::move(std::optional<T>{});
				return true;
			}
			front_container[index] = std::move(bufferPool.back());
			bufferPool.pop_back();
			return false;
		}
	};
}

namespace std {
	template<typename T, std::size_t ArraySize>
	class iterator_traits<ska::unique_buffered_priority_indexed_fixed_queue_iterator<T, ArraySize>> {
	public:
    	using difference_type = std::ptrdiff_t;
    	using size_type = std::size_t;
    	using value_type = T;
    	using pointer = T*;
    	using reference = T&;
    	using iterator_category = std::random_access_iterator_tag;
	};
}
