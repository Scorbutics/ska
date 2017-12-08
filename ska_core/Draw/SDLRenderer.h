#pragma once
#include <SDL.h>
#include "../Rectangle.h"
#include "../Point.h"
#include "Color.h"
#include "Renderer.h"

namespace ska {
	struct Color;
	class SDLSurface;
    class SDLTexture;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Renderer instance
	*/
	class SDLRenderer : public Renderer {
	public:
	    SDLRenderer();
		SDLRenderer(SDL_Window * window, int index, Uint32 flags);
		
		SDLRenderer(SDLRenderer&& r) = default;
		SDLRenderer& operator=(SDLRenderer&& r) = default;
		SDLRenderer(const SDLRenderer& r) = delete;
        SDLRenderer& operator=(const SDLRenderer& r) = delete;
		void setRenderColor(const ska::Color& c);

		static void setDefaultRenderer(SDLRenderer* renderer);
        static SDLRenderer* getDefaultRenderer();

        void renderClear() const;

        SDL_Texture* createTextureFromSurface(const SDLSurface& s) const override;
        void renderPresent() const;
        

		void drawColorPoint(const Color& c, const Point<int>& pos) const;
		void drawColorRect(const Color& c, const Rectangle& r) const;
		void drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const;

		~SDLRenderer();

		void load(SDL_Window * window, int index, Uint32 flags);

		SDL_Renderer* unwrap() const;
	private:
	    void free();
	
		protected:
		void render(SDLTexture& t, int posX, int posY, Rectangle const*) const override;
		void render(GifTexture& t, int posX, int posY, Rectangle const*) const override;

	private:
		SDL_Renderer* m_renderer;
		Color m_currentRenderColor;
		static SDLRenderer* m_currentDefaultRenderer;

	};
}
