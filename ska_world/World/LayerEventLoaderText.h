#pragma once
#include "LayerEventLoader.h"

namespace ska {
	class LayerEventLoaderText : 
		public LayerEventLoader {
	
	public:
		explicit LayerEventLoaderText(std::string layerFileName);
		std::vector<BlockEvent> load() const override;

	private:
		std::string m_fileName;
	};
}
