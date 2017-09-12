#include "../Exceptions/IllegalArgumentException.h"
#include "Window.h"
#include "../Draw/SDLRenderer.h"
#include "../Logging/Logger.h"
#include <SDL_image.h>

#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13

ska::Window::Window(const std::string& title, unsigned int w, unsigned int h) :
	m_height(h < TAILLEECRANMINY ? TAILLEECRANMINY : h),
	m_width(w < TAILLEECRANMINX ? TAILLEECRANMINX : w),
	m_wName(title),
	m_containsDefaultRenderer(false), m_iconFile(nullptr){
	m_screen = SDL_CreateWindow(title.c_str(),
	                            SDL_WINDOWPOS_UNDEFINED,
	                            SDL_WINDOWPOS_UNDEFINED,
	                            w, h,
	                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (m_screen == nullptr)
	{
		SKA_LOG_ERROR("Erreur lors de la création de la fenêtre SDL :", SDL_GetError());
		throw IllegalArgumentException("Bad instanciation : screen cannot be null");
	}

	m_renderer.load(m_screen, -1, SDL_RENDERER_ACCELERATED);

	if (SDLRenderer::getDefaultRenderer() == nullptr)
	{
		m_containsDefaultRenderer = true;
		SDLRenderer::setDefaultRenderer(&m_renderer);
	}
}

void ska::Window::display() const{
	m_renderer.renderPresent();
	m_renderer.renderClear();
}

ska::SDLRenderer& ska::Window::getRenderer() {
	return m_renderer;
}

void ska::Window::setWindowIcon(const std::string& filename) {
	SDL_FreeSurface(m_iconFile);
	m_iconFile = IMG_Load(filename.c_str());

	SDL_SetWindowIcon(m_screen, m_iconFile);
}

void ska::Window::setRenderColor(const Color & color) {
	m_renderer.setRenderColor(color);
}

void ska::Window::showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const {
    SDL_ShowSimpleMessageBox(flags,
            title.c_str(),
            message.c_str(),
            m_screen);
}

unsigned int ska::Window::getWidth() const {
	return m_width;
}

unsigned int ska::Window::getHeight() const {
	return m_height;
}

void ska::Window::resize(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;
}

ska::Window::~Window() {
    if(m_containsDefaultRenderer) {
        SDLRenderer::setDefaultRenderer(nullptr);
    }
	SDL_FreeSurface(m_iconFile);
	SDL_DestroyWindow(m_screen);
}
