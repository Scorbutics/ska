#pragma once
#include "Draw/Renderer.h"
#include "Point.h"
#include "Draw/Color.h"
#include <SDL.h>

namespace ska {
	class SDLWindow;
	struct Color;
	class SDLSurface;
    class SDLTexture;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Renderer instance
	*/
	class SDLRenderer : public Renderer {
	public:
		SDLRenderer(SDLWindow& window, int index, Uint32 flags);

		SDLRenderer(SDLRenderer&& r) = default;
		SDLRenderer& operator=(SDLRenderer&& r) = default;
		SDLRenderer(const SDLRenderer& r) = delete;
        SDLRenderer& operator=(const SDLRenderer& r) = delete;

		/**
		* \brief Changes the current default rendering color
		* \param color New color to set
		*/
		void setRenderColor(const Color& c);
		void setRenderTarget(Texture* t) override;
		void setRenderDrawBlendMode(int blendMode) override;

        SDL_Texture* createTextureFromSurface(const SDLSurface& s) const override;

		void drawColorPoint(const Color& c, const Point<int>& pos) const override;
		void drawColorRect(const Color& c, const Rectangle& r) const override;
		void drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const override;

		~SDLRenderer();

		SDL_Renderer* unwrap() const;

		/**
		* \brief Makes the renderer renders
		*/
		void render(const Texture& t, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const override;

		/**
		* \brief Makes the renderer renders
		*/
		void render(const AnimatedTexture& t, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const override;

		SDL_Texture* createTextureTarget(unsigned int width, unsigned int height) const override;

		void update() const override;

		void load() override;

	private:
		void load(SDL_Window * window, int index, Uint32 flags);
	    void free();
		void renderPresent() const;
		void renderClear() const;

		SDL_Renderer* m_renderer;
		Color m_currentRenderColor;
		SDLWindow& m_window;
		int m_index;
		uint32_t m_flags;

	};
}
