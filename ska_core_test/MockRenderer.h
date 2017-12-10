#pragma once
#include "Draw/Renderer.h"

class MockRenderer : public ska::Renderer {
public:
	SDL_Texture* createTextureFromSurface(const ska::SDLSurface& surface) const override;

	void render(const ska::Texture& t, int posX, int posY, ska::Rectangle const*) const override;
	void render(const ska::AnimatedTexture& t, int posX, int posY, ska::Rectangle const*) const override;
	void update() const override {}
	void drawColorPoint(const ska::Color& c, const ska::Point<int>& pos) const override;
	void drawColorRect(const ska::Color& c, const ska::Rectangle& r) const override;
	void drawColorLine(const ska::Color& c, const ska::Point<int>& p1, const ska::Point<int>& p2) const override;
};
