#pragma once
#include "LayerEventLoader.h"

namespace ska {
	class LayerEventLoaderText :
		public LayerEventLoader {

	public:
		explicit LayerEventLoaderText(std::string layerFileName);
		ska::Vector2<std::optional<BlockEvent>> load(unsigned int width, unsigned int height) const override;
        const std::string& getName() const override;

	private:
		std::string m_fileName;
	};
}
