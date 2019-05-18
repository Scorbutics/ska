#include <SDL_ttf.h>

#include "Graphic/__internalConfig/LoggerConfig.h"
#include "SDLFont.h"
#include "Core/SDLLibrary.h"
#include "Core/Exceptions/FileException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Graphic/SDLTTFLibrary.h"

std::string ska::SDLFont::DEFAULT_FONT_FILE = "resources/Fonts/FiraSans-Medium.ttf";

ska::SDLFont::SDLFont() {
	m_fontC = nullptr;
	open(DEFAULT_FONT_FILE, 12);
}

ska::SDLFont::SDLFont(unsigned int fontSize) {
	m_fontC = nullptr;
	open(DEFAULT_FONT_FILE, fontSize);
}

void ska::SDLFont::open(const std::string& fontFile, unsigned int fontSize) {
	SDLTTFLibrary::get().closeFont(m_fontC);
	m_fontC = SDLTTFLibrary::get().openFont(fontFile.c_str(), fontSize);
	if (m_fontC == nullptr) {
		ExceptionTrigger<ska::FileException>("Erreur lors de l'ouverture de la police : " + std::string(SDLLibrary::get().getError()));
	}
}

TTF_Font* ska::SDLFont::getInstance() {
	return m_fontC;
}

ska::SDLFont::~SDLFont() {
	SDLTTFLibrary::get().closeFont(m_fontC);
}
