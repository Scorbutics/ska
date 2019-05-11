#pragma once
#include "LayerEventLoader.h"
#include "Core/ECS/Basics/Script/BlockEvent.h"

namespace ska {
	class LayerEventLoaderText :
		public LayerEventLoader {

	public:
		explicit LayerEventLoaderText(std::string layerFileName);
		
		Vector2<ScriptPack> loadPositioned(unsigned int width, unsigned int height) const override;
		ScriptGlobalPack loadGlobal() const override;
        const std::string& getName() const override;

	private:
		std::pair<BlockEvent, ScriptSleepComponent> buildFromLine(const std::string& line, unsigned int lineIndex) const;
		std::string m_fileName;
		std::vector<std::string> m_fileContent;
	};
}
