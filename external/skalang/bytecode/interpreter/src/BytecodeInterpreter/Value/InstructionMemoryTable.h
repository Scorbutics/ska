#pragma once

#include <unordered_map>

namespace ska {
  namespace bytecode {
	using InstructionMemoryTable = std::unordered_map<std::size_t, std::size_t>;
  }
}
