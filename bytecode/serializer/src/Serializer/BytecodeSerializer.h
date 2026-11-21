#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeSerializationStrategy.h"
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {

	namespace bytecode {
		struct DeserializationContext;
		struct SerializationContext;
		class ScriptCache;

		class Serializer {
		public:
			// Version of the serialized bytecode format : a script serialized with another version is not deserialized
			// (it is reported as a failed script, to be rebuilt from its source).
			// 2 : call frames (LOCAL, UPVAL, SELF operands, CELL, PUSH_CELL, ARR_SET commands), serialized "exported" symbol flags,
			//     symbol tables keyed by child position, exact floating point values.
			// 3 : "return" leaves the function where it is (RET generated for each return statement of a function).
			// 4 : CMP_BOOL command (the commands after it are renumbered).
			// 5 : MOD_I command (the commands after it are renumbered).
			// 6 : "while" keyword (the grammar tokens after it are renumbered), STR_LENGTH and STR_ACCESS commands.
			// 7 : CONV_B_STR command.
			// 8 : maps (MAP_* commands, "map" type).
			static constexpr std::size_t Version = 8;

			Serializer() = default;
			// Reads and writes the given format version (tests of version mismatches)
			explicit Serializer(std::size_t version) : m_version(version) {}
			~Serializer() = default;

			bool serialize(SerializationContext& context) const;
			bool serialize(ScriptCache& cache, SerializationStrategy output) const;
			std::vector<std::string> deserialize(DeserializationContext& output, const std::unordered_set<std::string>& blacklist = {}) const;
			std::vector<std::string> deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input, const std::unordered_set<std::string>& blacklist = {}) const;

		private:
			std::vector<std::string> deserialize(DeserializationContext& context, const std::string& scriptName, const std::unordered_set<std::string>& blacklist) const;

			std::size_t m_version = Version;
		};
	}
}
