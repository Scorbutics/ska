#include <doctest.h>
#include <functional>
#include "Base/Serialization/SerializerType.h"

struct Toto {
	int a;
	std::string lol;
};

struct Bidule {
	long l;
};

struct Titi {
	uint64_t id;
	Toto toto;
	Bidule* bidule;
};

namespace ska {

	template <>
	struct SerializerTypeTraits<std::string> {
		static constexpr std::size_t BytesRequired = sizeof(int64_t);
		static constexpr const char* Name = "std::string";
	};

	template <>
	struct SerializerTypeTraits<Toto> {
		static constexpr std::size_t BytesRequired = sizeof(int) + SerializerTypeTraits<std::string>::BytesRequired;
		static constexpr const char* Name = "Toto";

		static void Read(SerializerSafeZone<BytesRequired>& zone, Toto& toto) {
			toto.a = zone.read<int>();
			toto.lol = zone.read<std::string>();
		}

		static void Write(SerializerSafeZone<BytesRequired>& zone, const Toto& toto) {
			zone.write(toto.a);
			zone.write(toto.lol);
		}
	};

	template <>
	struct SerializerTypeTraits<Titi> {
		static constexpr std::size_t BytesRequired = SerializerTypeTraits<Toto>::BytesRequired + sizeof(uint64_t);
		static constexpr const char* Name = "Titi";

		static void Read(SerializerSafeZone<BytesRequired>& zone, Titi& titi) {
			SerializerTypeTraits<Toto>::Read(zone.acquireMemory<SerializerTypeTraits<Toto>::BytesRequired>("Toto"), titi.toto);
			titi.id = zone.read<uint64_t>();
		}

		static void Write(SerializerSafeZone<BytesRequired>& zone, const Titi& titi) {
			SerializerTypeTraits<Toto>::Write(zone.acquireMemory<SerializerTypeTraits<Toto>::BytesRequired>("Toto"), titi.toto);
			zone.write(titi.id);
			//zone.write(titi.bidule);
		}
	};
}

TEST_CASE("[SerializerType] test") {
	std::stringstream ss_;
	ska::SerializerNativeContainer natives_;
	ska::SerializerOutput output {ska::SerializerOutputData{ss_, natives_}};

	std::function<void(const std::vector<ska::SerializerErrorData>&)> checkErrorCallback;
	checkErrorCallback = [](const std::vector<ska::SerializerErrorData>& errors) {
		CHECK(false);
	};
	
	SUBCASE("toto") {
		auto totoSerializer = ska::SerializerType<Toto>{ output };

		Toto t{ 0, "mdr" };

		totoSerializer.write(t);
		totoSerializer.reset();
		Toto t2;
		totoSerializer.read(t2);

		CHECK(t2.a == t.a);
		CHECK(t2.lol == t.lol);
	}

	SUBCASE("titi") {
		auto titiSerializer = ska::SerializerType<Titi>{ output };

		Titi t{ 123, {4, "ll"}, nullptr };

		titiSerializer.write(t);
		titiSerializer.reset();
		Titi t2;
		titiSerializer.read(t2);

		CHECK(t2.id == t.id);
		CHECK(t2.toto.a == t.toto.a);
		CHECK(t2.toto.lol == t.toto.lol);
	}

	try {
		output.validateOrThrow();
	} catch (const ska::SerializationMemoryException& exception) {
		checkErrorCallback(exception.data);
	}
}
