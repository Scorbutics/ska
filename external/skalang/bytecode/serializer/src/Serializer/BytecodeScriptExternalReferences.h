#pragma once
#include <cstddef>
#include <unordered_set>
#include "BytecodeChunk.h"
#include "Base/Serialization/SerializerNatives.h"
namespace ska {
	namespace bytecode {
		struct ScriptHeader;
		struct ScriptBody;

		struct ScriptExternalReferences {
			ScriptExternalReferences(const SerializerNativeContainer& natives) : m_natives(natives) {}
			const SerializerNativeContainer& natives() const { return m_natives; }
			std::unordered_set<std::string> scripts{};
		private:
			const SerializerNativeContainer& m_natives;
		};
	}
}
