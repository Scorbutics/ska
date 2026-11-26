#pragma once
#include <memory>

namespace ska {
	class MemoryTable;
	using ObjectMemory = std::shared_ptr<MemoryTable>;
}