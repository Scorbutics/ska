#pragma once
#include <vector>
#include <deque>
#include <ostream>
#include <memory>
#include "NodeValue/Token.h"

namespace ska {
	namespace bytecode {
		class InstructionOutput;
		class ScriptGeneration;
		class Instruction;
		class ScriptCache;
		class InstructionsDebugInfo {
		public:
			InstructionsDebugInfo(ScriptGeneration& generation, std::size_t columnWidth = 100);
			InstructionsDebugInfo(std::string scriptData, std::size_t columnWidth = 100);

			InstructionsDebugInfo(InstructionsDebugInfo&&) noexcept = default;
			InstructionsDebugInfo(const InstructionsDebugInfo&) = delete;
			InstructionsDebugInfo& operator=(InstructionsDebugInfo&&) = default;
			InstructionsDebugInfo& operator=(const InstructionsDebugInfo&) = delete;

			void print(std::ostream& stream, ScriptCache& cache, std::size_t id) const;
			void print(std::ostream& stream, const InstructionOutput& generation) const;
		private:
			std::string m_scriptFileContent;
			std::size_t m_columnWidth;
		};
	}
}
