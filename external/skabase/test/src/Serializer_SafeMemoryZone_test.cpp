#include <doctest.h>
#include <functional>
#include "Base/Serialization/SerializerOutput.h"

TEST_CASE("[Serializer] with throw") {
	std::stringstream ss_;
	ska::SerializerNativeContainer natives_;
	ska::SerializerOutput output {ska::SerializerOutputData{ss_, natives_}};

	std::function<void(const std::vector<ska::SerializerErrorData>&)> checkErrorCallback;

	SUBCASE("not enough bytes written") {
		auto memory = output.acquireMemory<8>("13");

		memory.write(uint8_t{ 1 });
		memory.write(uint16_t{ 1 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 1);
			CHECK(errors[0].bytesRequested == 0);
			CHECK(errors[0].bytesAllocated == 3);
			CHECK(errors[0].bytesAvailable == 8);
			CHECK(errors[0].zone == "13");
		};
	}

	SUBCASE("too many bytes written") {
		auto memory = output.acquireMemory<2>("27.1");

		memory.write(uint8_t{ 1 });
		memory.write(uint16_t{ 1 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 1);
			CHECK(errors[0].bytesRequested == 0);
			CHECK(errors[0].bytesAllocated == 3);
			CHECK(errors[0].bytesAvailable == 2);
			CHECK(errors[0].zone == "27.1");
		};
	}

	SUBCASE("[children] 1 child too many bytes written in child") {
		auto memory = output.acquireMemory<4>("27");
		auto submemory = memory.acquireMemory<2>("part 1");
		
		submemory.write(uint8_t{1});
		submemory.write(uint16_t{1});
		
		memory.write(uint8_t {3});

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 1);
			CHECK(errors[0].bytesRequested == 0);
			CHECK(errors[0].bytesAllocated == 3);
			CHECK(errors[0].bytesAvailable == 2);
			CHECK(errors[0].zone == "27 | part 1");
		};
	}

	SUBCASE("[children] 1 child not enough bytes written in child") {
		auto memory = output.acquireMemory<4>("58");
		auto submemory = memory.acquireMemory<2>("part 1");
		submemory.write(uint8_t{ 1 });

		memory.write(uint8_t{ 3 });
		memory.write(uint16_t{ 3 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 1);
			CHECK(errors[0].bytesRequested == 0);
			CHECK(errors[0].bytesAllocated == 1);
			CHECK(errors[0].bytesAvailable == 2);
			CHECK(errors[0].zone == "58 | part 1");
		};
	}

	SUBCASE("[children] 1 child write on child then overwrite on parent") {
		auto memory = output.acquireMemory<4>("92");
		auto submemory = memory.acquireMemory<4>("part 1");
		submemory.write(uint32_t{ 3 });

		memory.write(uint8_t{ 1 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 1);
			CHECK(errors[0].bytesRequested == 0);
			CHECK(errors[0].bytesAllocated == 5);
			CHECK(errors[0].bytesAvailable == 4);
			CHECK(errors[0].zone == "92");
		};
	}

	SUBCASE("[children] 2 children acquire too many bytes from parent and so parent write too many bytes") {
		auto memory = output.acquireMemory<4>("74");
		auto submemory = memory.acquireMemory<3>("part 1");
		auto submemory2 = memory.acquireMemory<2>("part 2");
		submemory.write(uint16_t{ 0 });
		submemory.write(uint8_t{ 0 });
		submemory2.write(uint16_t { 0 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 2);
			CHECK(errors[0].bytesRequested == 2);
			CHECK(errors[0].bytesAllocated == 3);
			CHECK(errors[0].bytesAvailable == 4);
			CHECK(errors[0].zone == "74 | part 2");

			CHECK(errors[1].bytesRequested == 0);
			CHECK(errors[1].bytesAllocated == 5);
			CHECK(errors[1].bytesAvailable == 4);
			CHECK(errors[1].zone == "74");
		};
	}

	SUBCASE("[children] cannot acquire bytes for a second child + too many in a child") {
		auto memory = output.acquireMemory<4>("23");
		auto submemory = memory.acquireMemory<2>("part 1");

		submemory.write(uint8_t {1});
		submemory.write(uint8_t {2});

		auto submemory2 = memory.acquireMemory<3>("part 2");
		submemory2.write(uint16_t {1});
		submemory2.write(uint8_t {2});

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 2);
			CHECK(errors[0].bytesRequested == 3);
			CHECK(errors[0].bytesAllocated == 2);
			CHECK(errors[0].bytesAvailable == 4);
			CHECK(errors[0].zone == "23 | part 2");

			CHECK(errors[1].bytesRequested == 0);
			CHECK(errors[1].bytesAllocated == 5);
			CHECK(errors[1].bytesAvailable == 4);
			CHECK(errors[1].zone == "23");
		};
	}

	SUBCASE("[children] 2 children request for a child but unused, after filling parent memory, then use correctly second child") {
		auto memory = output.acquireMemory<5>("40");

		memory.write(uint8_t{ 1 });
		memory.write(uint8_t{ 2 });

		auto submemory = memory.acquireMemory<2>("part 1");

		auto submemory2 = memory.acquireMemory<3>("part 2");
		submemory2.write(uint16_t{ 1 });
		submemory2.write(uint8_t{ 2 });

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(errors.size() == 2);
			CHECK(errors[0].bytesRequested == 3);
			CHECK(errors[0].bytesAllocated == 4);
			CHECK(errors[0].bytesAvailable == 5);
			CHECK(errors[0].zone == "40 | part 2");

			CHECK(errors[1].bytesRequested == 0);
			CHECK(errors[1].bytesAllocated == 0);
			CHECK(errors[1].bytesAvailable == 2);
			CHECK(errors[1].zone == "40 | part 1");
		};
	}

	SUBCASE("[children] 2 children OK") {
		auto memory = output.acquireMemory<5>("40");
		auto submemory = memory.acquireMemory<2>("part 1");

		submemory.write(uint8_t {1});
		submemory.write(uint8_t {2});

		auto submemory2 = memory.acquireMemory<3>("part 2");
		submemory2.write(uint16_t {1});
		submemory2.write(uint8_t {2});

		checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
			CHECK(false);
		};
	}

	try {
		output.validateOrThrow();
	} catch (const ska::SerializationMemoryException& exception) {
		checkErrorCallback(exception.data);
	}
}
