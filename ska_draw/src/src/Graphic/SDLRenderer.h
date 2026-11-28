#pragma once
#include "Core/Draw/Renderer.h"
#include "Core/Point.h"
#include "Core/Draw/Color.h"
#include <SDL3/SDL.h>
#include <cstdint>

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
		// The "gpu" render driver when it can be used (the shaders need it), else the best one of SDL. The environment
		// variable SDL_RENDER_DRIVER chooses another one
		explicit SDLRenderer(SDLWindow& window);

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
		void renderRegion(const Texture& t, float x, float y, float w, float h, const Rectangle& destination) const override;

		/**
		* \brief Makes the renderer renders
		*/
		void render(const AnimatedTexture& t, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const override;

		SDL_Texture* createTextureTarget(unsigned int width, unsigned int height) const override;

		void update() const override;

		void load() override;
		void setScreenShader(const Shader* shader) override;
		void beginWorld(float zoom) override;
		void endWorld() override;

	private:
		void load(SDL_Window* window);
	    void free();
		void renderPresent() const;
		// Screenshots asked by the development console
		void takeConsoleShots() const;
		void renderClear() const;

		SDL_Renderer* m_renderer;
		Color m_currentRenderColor;
		SDLWindow& m_window;
		// Post-processing : the frame is drawn in this texture, then on the screen with the shader
		const Shader* m_screenShader = nullptr;
		mutable SDL_Texture* m_screenTarget = nullptr;
		mutable int m_screenTargetWidth = 0;
		mutable int m_screenTargetHeight = 0;
		// The frame being drawn goes in the texture (the shader is used from the next frame)
		mutable bool m_frameInTarget = false;
		void prepareScreenTarget() const;
		// The world zoomed : drawn in this texture (the size of the screen divided by the zoom)
		SDL_Texture* m_worldTarget = nullptr;
		int m_worldTargetWidth = 0;
		int m_worldTargetHeight = 0;
		// The part of the world texture drawn this frame (the texture can be larger : it is not created again at each zoom)
		SDL_Rect m_worldArea{};
		bool m_inWorld = false;

	};
}
