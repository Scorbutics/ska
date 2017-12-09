#pragma once
#include <SDL.h>
#include "Draw/Renderer.h"
#include "Point.h"
#include "Draw/Color.h"

namespace ska {
	class Window;
	struct Color;
	class SDLSurface;
    class SDLTexture;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Renderer instance
	*/
	class SDLRenderer : public Renderer {
	public:
		SDLRenderer(Window& window, int index, Uint32 flags);
		
		SDLRenderer(SDLRenderer&& r) = default;
		SDLRenderer& operator=(SDLRenderer&& r) = default;
		SDLRenderer(const SDLRenderer& r) = delete;
        SDLRenderer& operator=(const SDLRenderer& r) = delete;

		/**
		* \brief Changes the current default rendering color
		* \param color New color to set
		*/
		void setRenderColor(const Color& c);

        SDL_Texture* createTextureFromSurface(const SDLSurface& s) const override;
        
		void drawColorPoint(const Color& c, const Point<int>& pos) const override;
		void drawColorRect(const Color& c, const Rectangle& r) const override;
		void drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const override;

		~SDLRenderer();

		void load(SDL_Window * window, int index, Uint32 flags);

		SDL_Renderer* unwrap() const;

		/**
		* \brief Makes the renderer renders
		*/
		void render(const Texture& t, int posX, int posY, Rectangle const*) const override;
		
		/**
		* \brief Makes the renderer renders
		*/
		void render(const AnimatedTexture& t, int posX, int posY, Rectangle const*) const override;

		void update() const override;

	private:
	    void free();
		void renderPresent() const;
		void renderClear() const;
		
		SDL_Renderer* m_renderer;
		Color m_currentRenderColor;

	};
}
