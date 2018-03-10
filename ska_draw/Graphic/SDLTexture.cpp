#include "SDLTexture.h"
#include "SDLSurface.h"
#include "TextureData.h"
#include "Font.h"
#include "Task/WorkNode.h"
#include "Task/TaskQueue.h"
#include "Draw/Renderer.h"
#include "SDL_RectConverter.h"

ska::SDLTexture::SDLTexture(TextureData& data) :
	m_texture(nullptr),
	m_w(0), 
	m_h(0) {

	m_surface = std::make_unique<SDLSurface>();

	const auto& color = data.getData().second;

	switch (data.type) {
	case EnumTextureType::TEXT:
		m_surface->loadFromText(Font(data.fontSize), data.getData().first, data.getData().second);
		break;
	case EnumTextureType::SPRITE:
		m_surface->load(data.getData().first, &color);
		break;
	case EnumTextureType::RECT:
		m_surface->loadFromColoredRect(data.getData().second, ToSDL_Rect(data.rect));
		break;
	default:
		break;
	}
	if (m_surface->getInstance() != nullptr) {
		m_h = m_surface->getInstance()->h;
		m_w = m_surface->getInstance()->w;
	} else {
		m_surface = nullptr;
	}
	m_whenLoadedTasks = std::make_unique<TaskQueue>();
}


void ska::SDLTexture::loadFromRenderer(const Renderer& renderer) {
	free();

	if(m_surface == nullptr) {
		return;
	}

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
}

ska::SDLTexture::~SDLTexture() {
	free();
}

void ska::SDLTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
	auto action = [&]() {
		SDL_SetTextureColorMod(m_texture, r, g, b);
		return false;
	};

	if (isInitialized()) {
		(action)();
	} else {
		m_whenLoadedTasks->queueTask(action);
	}
}

void ska::SDLTexture::setBlendMode(int blending) const {
	auto action = [&]() {
		SDL_SetTextureBlendMode(m_texture, static_cast<SDL_BlendMode>(blending));
		return false;
	};

	if (isInitialized()) {
		(action)();
	} else {
		m_whenLoadedTasks->queueTask(std::move(action));
	}
}

void ska::SDLTexture::setAlpha(Uint8 alpha) const {
	auto action = [&]() {
		SDL_SetTextureAlphaMod(m_texture, alpha);
		return false;
	};

	if (isInitialized()) {
		(action)();
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

void ska::SDLTexture::load(const Renderer& renderer) {
	if(!isInitialized()) {
		loadFromRenderer(renderer);
	}
}


