#pragma once
#include <optional>
#include "Utils/Vector2.h"
#include "BlockEvent.h"

namespace ska {
	class LayerEventLoader {
	public:
		virtual ~LayerEventLoader() = default;

		virtual ska::Vector2<std::optional<BlockEvent>> load(unsigned int width, unsigned int height) const = 0;
		virtual const std::string& getName() const = 0;
	};
}
