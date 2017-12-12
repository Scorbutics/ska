#pragma once
#include <memory>
#include "SDLSurface.h"
#include "Task/TaskQueue.h"

namespace ska {
	class Renderer;
	class Texture;
	class TextureData;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Texture instance
	*/
	class SDLTexture {
		friend class SDLRenderer;
		friend class Renderer;
	public:
		
		explicit SDLTexture(TextureData& p);

		SDLTexture(SDLTexture&) = delete;
		SDLTexture& operator=(const SDLTexture&) = delete;

		void free();
		~SDLTexture();

		void setColor(Uint8 uint8, Uint8 uint9, Uint8 uint81);
		void setBlendMode(int blending) const;
		void setAlpha(Uint8 alpha) const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;
		void resize(unsigned width, unsigned height);
		void load(const Renderer& renderer);

	private:
		void loadFromRenderer(const Renderer& renderer);
		bool isInitialized() const;

		using SDLSurfacePtr = std::unique_ptr<SDLSurface>;		
		using DeferredTasksPtr = std::unique_ptr<TaskQueue>;
		
		SDLSurfacePtr m_surface;
		SDL_Texture* m_texture;
		unsigned int m_w;
		unsigned int m_h;

		DeferredTasksPtr m_whenLoadedTasks;
	};
}

