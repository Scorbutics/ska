#include <SDL_image.h>

#include "SDLWindow.h"
#include "Exceptions/IllegalArgumentException.h"
#include "Logging/Logger.h"

#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13

ska::SDLWindow::SDLWindow(const std::string& title, unsigned int w, unsigned int h) :
	m_height(h < TAILLEECRANMINY ? TAILLEECRANMINY : h),
	m_width(w < TAILLEECRANMINX ? TAILLEECRANMINX : w),
	m_wName(title),
	m_iconFile(nullptr) {
	m_screen = SDL_CreateWindow(m_wName.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

	if (m_screen == nullptr) {
		SKA_LOG_ERROR("Erreur lors de la création de la fenêtre SDL :", SDL_GetError());
		throw IllegalArgumentException("Bad instanciation : screen is null");
	}
}

void ska::SDLWindow::show() {
	if (m_screen != nullptr) {
		SDL_ShowWindow(m_screen);
	} else {
		SKA_LOG_INFO("Window already shown");
	}
}

void ska::SDLWindow::setWindowIcon(const std::string& filename) {
	SDL_FreeSurface(m_iconFile);
	m_iconFile = IMG_Load(filename.c_str());

	SDL_SetWindowIcon(m_screen, m_iconFile);
}

void ska::SDLWindow::showMessageBox(uint32_t flags, const std::string& title, const std::string& message) const {
    SDL_ShowSimpleMessageBox(flags,
            title.c_str(),
            message.c_str(),
            m_screen);
}

unsigned int ska::SDLWindow::getWidth() const {
	return m_width;
}

unsigned int ska::SDLWindow::getHeight() const {
	return m_height;
}

void ska::SDLWindow::resize(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;
}

ska::SDLWindow::~SDLWindow() {
	SDL_FreeSurface(m_iconFile);
	SDL_DestroyWindow(m_screen);
}
