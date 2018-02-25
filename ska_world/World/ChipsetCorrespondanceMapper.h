#pragma once
#include <string>
#include "Point.h"
#include "Draw/Color.h"
#include "Graphic/SDLSurface.h"

namespace ska {
	class ChipsetCorrespondanceMapper {
	public:
		explicit ChipsetCorrespondanceMapper(const std::string& filenameCorr);
		~ChipsetCorrespondanceMapper() = default;

		const std::unordered_map<Color, Point<int>>& access() const;
		unsigned int getFileWidth() const;

	private:
		void buildCorrMap(const SDLSurface& fichierMCorr);

		std::unordered_map<Color, Point<int>> m_corr;
		unsigned int m_fileWidth;
	};
}
