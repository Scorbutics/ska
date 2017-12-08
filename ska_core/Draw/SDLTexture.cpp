#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "SDLSurface.h"
#include "TextureData.h"
#include "Font.h"
#include "../Task/WorkNode.h"
#include "../Task/TaskQueue.h"
#include "Renderer.h"

ska::SDLTexture::SDLTexture() : 
	m_texture(nullptr),
	m_w(0), 
	m_h(0) {
	m_whenLoadedTasks = std::make_unique<TaskQueue>();
}

ska::SDLTexture::SDLTexture(TextureData& data) :
	m_texture(nullptr),
	m_w(0), 
	m_h(0) {

	m_surface = std::make_unique<SDLSurface>();

	const auto& color = data.getData().second;

	switch (data.type) {
	case TEXT:
		m_surface->loadFromText(Font(data.fontSize), data.getData().first, data.getData().second);
		break;
	case SPRITE:
		m_surface->load(data.getData().first, &color);
		break;
	case RECT:
		m_surface->loadFromColoredRect(data.getData().second, data.rect);
		break;
	default:
		break;
	}
	m_h = m_surface->getInstance()->h;
	m_w = m_surface->getInstance()->w;
	m_whenLoadedTasks = std::make_unique<TaskQueue>();
}


void ska::SDLTexture::loadFromRenderer(const Renderer& renderer) {
	free();

	m_texture = renderer.createTextureFromSurface(*m_surface);
	m_surface = nullptr;

	while (m_whenLoadedTasks->hasRunningTask()) {
		m_whenLoadedTasks->refresh();
	}
	
}

bool ska::SDLTexture::isInitialized() const {
	return m_texture != nullptr && m_surface == nullptr;
}

void ska::SDLTexture::free() {
	if (m_texture != nullptr) {
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
	m_h = m_w = 0;	
}

ska::SDLTexture::~SDLTexture() {
	free();
}

void ska::SDLTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
	auto action = std::make_unique<Runnable>([&]() {
		SDL_SetTextureColorMod(m_texture, r, g, b);
		return false;
	});

	if (isInitialized()) {
		(*action)();
	} else {
		m_whenLoadedTasks->queueTask(std::move(action));
	}
}

void ska::SDLTexture::setBlendMode(int blending) const {
	auto action = std::make_unique<Runnable>([&]() {
		SDL_SetTextureBlendMode(m_texture, static_cast<SDL_BlendMode>(blending));
		return false;
	});

	if (isInitialized()) {
		(*action)();
	} else {
		m_whenLoadedTasks->queueTask(std::move(action));
	}
}

void ska::SDLTexture::setAlpha(Uint8 alpha) const {
	auto action = std::make_unique<Runnable>([&]() {
		SDL_SetTextureAlphaMod(m_texture, alpha);
		return false;
	});

	if (isInitialized()) {
		(*action)();
	} else {
		m_whenLoadedTasks->queueTask(std::move(action));
	}
}

unsigned ska::SDLTexture::getWidth() const{
	 return m_w;	
}

unsigned ska::SDLTexture::getHeight() const{	
	return m_h;
}

void ska::SDLTexture::resize(unsigned width, unsigned height) {
	m_w = width;
	m_h = height;
}

int ska::SDLTexture::load(const Renderer& renderer) {
	if(!isInitialized()) {
		loadFromRenderer(renderer);
	}
}


