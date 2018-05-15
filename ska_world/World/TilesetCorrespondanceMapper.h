#pragma once
#include <string>
#include <unordered_map>
#include "Point.h"
#include "Draw/Color.h"
#include "Graphic/SDLSurface.h"
#include "Utils/MovableNonCopyable.h"

namespace ska {
	class TilesetCorrespondanceMapper : public MovableNonCopyable {
	public:
		explicit TilesetCorrespondanceMapper(const std::string& filenameCorr);
		TilesetCorrespondanceMapper(TilesetCorrespondanceMapper&&) = default;
		~TilesetCorrespondanceMapper() = default;

		const std::unordered_map<Color, Point<int>>& access() const;
		unsigned int getFileWidth() const;

	private:
		void buildCorrMap(const SDLSurface& fichierMCorr);

		std::unordered_map<Color, Point<int>> m_corr;
		unsigned int m_fileWidth;
	};
}
