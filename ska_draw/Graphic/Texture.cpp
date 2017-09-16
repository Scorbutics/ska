#include "Texture.h"
#include "Draw/Color.h"
#include "Draw/SDLRenderer.h"
#include "Exceptions/IllegalStateException.h"

ska::SDLRenderer* ska::Texture::m_renderer = nullptr;

ska::Texture::Texture(const std::string& id, int r, int g, int b, int a) : 
	ManagedResource() {
	load(id, r, g, b, a);
}

void ska::Texture::load(const std::string& id, int r, int g, int b, int a) {
	checkRenderer();

	Color finalColor;
	finalColor.a = static_cast<Uint8>(a);
	finalColor.b = static_cast<Uint8>(b);
	finalColor.g = static_cast<Uint8>(g);
	finalColor.r = static_cast<Uint8>(r);

	loadFromKey(TextureData(*m_renderer, id, finalColor, false, 0));
}

void ska::Texture::setColor(Uint8 r, Uint8 g, Uint8 b) const {
	if (m_value != nullptr) {
		SDL_SetTextureColorMod(m_value->m_texture, r, g, b);
	}
}

void ska::Texture::setBlendMode(SDL_BlendMode blending) const {
	if (m_value != nullptr) {
		SDL_SetTextureBlendMode(m_value->m_texture, blending);
	}
}



void ska::Texture::setAlpha(Uint8 alpha) const{
	if (m_value != nullptr) {
		SDL_SetTextureAlphaMod(m_value->m_texture, alpha);
		m_value->m_alpha = alpha;
	}
}

int ska::Texture::render(int x, int y, const Rectangle* clip) const {
	if (m_value == nullptr) {
		return -1;
	}

	checkRenderer();

	Rectangle destBuf = { x, y, m_value->m_w, m_value->m_h };

	if( clip != nullptr ) {
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}

	return m_renderer->renderCopy(*m_value.get(), clip, destBuf);
}

void ska::Texture::resize(unsigned int width, unsigned int height) {
	m_value->m_w = width;
	m_value->m_h = height;
}

void ska::Texture::checkRenderer() {
	if (m_renderer == nullptr) {
        m_renderer = SDLRenderer::getDefaultRenderer();
        if(m_renderer == nullptr) {
            throw IllegalStateException("The Texture's default window is not set");
        }
	}
}

void ska::Texture::loadFromText(unsigned int fontSize, std::string text, Color c) {
	checkRenderer();

	loadFromKey(TextureData(*m_renderer, text, c, true, fontSize));
}

unsigned int ska::Texture::getWidth() const {
	return m_value == nullptr ? 0 : m_value->m_w;
}

unsigned int ska::Texture::getHeight() const {
	return m_value == nullptr ? 0 : m_value->m_h;
}



