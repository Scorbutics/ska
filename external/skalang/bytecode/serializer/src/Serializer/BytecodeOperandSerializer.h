#pragma once

#include <string>
#include <unordered_map>
#include "Base/Serialization/SerializerOutput.h"
#include "Serializer/BytecodeChunk.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	namespace bytecode {
		
		class ScriptCache;
		class OperandSerializer {
		public:
			OperandSerializer() = default;
			~OperandSerializer() = default;

			static void write(const ScriptCache& cache, SerializerSafeZone<sizeof(uint8_t) + sizeof(Chunk) * 2> output, const Operand& value);
			static Operand read(const ScriptCache& cache, SerializerSafeZone<2 * sizeof(Chunk) + sizeof(uint8_t)> input);
		private:
			static const std::string scriptName(const ScriptCache& cache, std::size_t id);
			static size_t scriptId(const ScriptCache& cache, const std::string& name);
		};
	}
}