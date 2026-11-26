#pragma once
#include <sstream>
#include <cstddef>
#include <unordered_map>
#include "Serializer/BytecodeChunk.h"
#include "Base/Serialization/SerializerOutput.h"

namespace ska {
	namespace bytecode {
		class CommonSerializer {
		public:
			CommonSerializer() = default;
			~CommonSerializer() = default;

			static void write(SerializerSafeZone<sizeof(uint32_t)> buffer, std::size_t value);
			
			template <std::size_t size>
			static void writeNullChunk(SerializerSafeZone<sizeof(Chunk) * size> buffer) {
				char empty[sizeof(Chunk) * size] = "";
				buffer.write(empty);
			}

			static void write(SerializerSafeZone<sizeof(Chunk)> output, std::string value);
		};
	}
}