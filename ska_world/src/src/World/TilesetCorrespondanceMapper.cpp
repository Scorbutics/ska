#include "Graphic/SDLSurface.h"
#include "Core/Point.h"
#include "Core/Exceptions/CorruptedFileException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"
#include "TilesetCorrespondanceMapper.h"
#include "Core/SDLLibrary.h"

ska::TilesetCorrespondanceMapper::TilesetCorrespondanceMapper(const std::string& filenameCorr) {
	SDLSurface surf;
	surf.load32(filenameCorr);
	if(surf.getInstance() == nullptr) {
		ExceptionTrigger<FileException>("Erreur lors du chargement de \"" + filenameCorr + "\", fichier de correspondance entre tuiles du monde et position dans le chipset. "
			"Sans ce fichier, le chargement d'un chipset (et donc d'un monde) est impossible. " + std::string(SDLLibrary::get().getError()));
	}
	m_fileWidth = surf.getInstance()->w;
	buildCorrMap(surf);
}

const std::unordered_map<ska::Color, ska::Point<int>>& ska::TilesetCorrespondanceMapper::access() const{
	return m_corr;
}

unsigned ska::TilesetCorrespondanceMapper::getFileWidth() const{
	return m_fileWidth;
}

void ska::TilesetCorrespondanceMapper::buildCorrMap(const SDLSurface& fichierMCorr) {
	for (auto x = 0; x < fichierMCorr.getInstance()->w; x++) {
		for (auto y = 0; y < fichierMCorr.getInstance()->h; y++) {
			auto c = fichierMCorr.getPixel32Color(x, y);
			if (m_corr.find(c) == m_corr.end()) {
				m_corr.insert(std::make_pair(c, Point<int>(x, y)));
			} else {
				auto& pos2 = m_corr[c];
				ExceptionTrigger<CorruptedFileException>("Chipset correspondance file (\"corr.png\") has several tiles with same color at ("
					+ StringUtils::intToStr(pos2.x) + "; " + StringUtils::intToStr(pos2.y) + ") and (" + StringUtils::intToStr(x) + "; " + StringUtils::intToStr(y)
					+ "). Please keep only one color correspondance per tile");
			}
		}
	}
}
