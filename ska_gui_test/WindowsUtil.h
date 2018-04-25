#pragma once
#include "Inputs/InputContextManager.h"
#include "Data/Events/GameEventDispatcher.h"
#include "InputContextTest.h"
#include "Draw/Renderer.h"

ska::GameEventDispatcher& GetGED();
ska::RawInputListener& GetRawInputListener();
InputContextTest* GetICT();
ska::InputContextManager& GetICM();

struct MockRenderer : public ska::Renderer {
	SDL_Texture* createTextureFromSurface(const ska::SDLSurface& surface) const override { return 0; }

	void render(const ska::Texture& t, int posX, int posY, ska::Rectangle const* clip, double angle = 0., ska::Point<int> const* rotationCenter = nullptr) const override {};
    void render(const ska::AnimatedTexture& at, int x, int y, ska::Rectangle const* clip, double angle = 0., ska::Point<int> const* rotationCenter = nullptr) const override {};
	void update() const override{}
	void drawColorPoint(const ska::Color& c, const ska::Point<int>& pos) const override {}
	void drawColorRect(const ska::Color& c, const ska::Rectangle& r) const override {}
	void drawColorLine(const ska::Color& c, const ska::Point<int>& p1, const ska::Point<int>& p2) const override {}
};

MockRenderer& MakeMockRenderer();
