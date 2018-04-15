#pragma once
#include <vector>
#include "BlockEvent.h"

namespace ska {
	class LayerEventLoader {
	public:
		virtual ~LayerEventLoader() = default;

		virtual std::vector<BlockEvent> load() const = 0;
	};
}
