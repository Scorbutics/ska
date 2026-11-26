#include <doctest.h>
#include "Container/unique_buffered_priority_indexed_fixed_queue.h"

template <class T>
struct PriorityValue {
	T value;
	std::size_t priority = 0u;
};


template <class Iterator>
void AssertContainValuesSplit(Iterator it) {}

template <class Iterator, int Value, int ... Values>
void AssertContainValuesSplit(Iterator it) {
	CHECK((*it).value == Value);
	AssertContainValuesSplit<Iterator, Values...>(++it);
}

template <class T, int ... Values>
void AssertContainValues(const T & c) {
	AssertContainValuesSplit<decltype(c.begin()), Values...>(c.begin());
}

TEST_CASE("[unique_buffered_priority_indexed_fixed_queue]") {
	using Queue = ska::unique_buffered_priority_indexed_fixed_queue<PriorityValue<int>, 3>;
	auto container = Queue{};

	container.push({ 10, 0 });
	AssertContainValues<Queue, 10>(container);

	container.push({ 123, 0 });
	AssertContainValues<Queue, 123>(container);

	container.push({ -1, 2 });
	AssertContainValues<Queue, 123, -1>(container);

	container.push({ 7, 1 });
	AssertContainValues<Queue, 123, 7, -1>(container);

	container.remove({ 7, 1 });
	AssertContainValues<Queue, 123, -1>(container);

	container.remove({ 123, 0 });
	AssertContainValues<Queue, 10, -1>(container);
}

TEST_CASE("[unique_buffered_priority_indexed_fixed_queue] not starting in index 0") {
	using Queue = ska::unique_buffered_priority_indexed_fixed_queue<PriorityValue<int>, 3>;
	auto container = Queue{};

	container.push({ -1, 2 });
	AssertContainValues<Queue, -1>(container);

	container.push({ 7, 1 });
	AssertContainValues<Queue,  7, -1>(container);

}