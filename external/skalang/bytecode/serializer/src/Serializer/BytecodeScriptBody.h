#pragma once
#include <cstddef>

#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeExport.h"
#include "Base/Serialization/SerializerNatives.h"

namespace ska {
	namespace bytecode {
		struct ScriptBody {
			ScriptBody(SerializerNativeContainer& natives) : m_natives(natives) {}

			const SerializerNativeContainer& natives() const { return m_natives; }
			SerializerNativeContainer& natives() { return m_natives; }

			std::vector<Instruction> instructions {};

		private:
			SerializerNativeContainer& m_natives;
		};
	}
}
