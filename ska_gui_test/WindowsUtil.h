#pragma once
#include "Inputs/InputContextManager.h"
#include "Data/Events/GameEventDispatcher.h"
#include "InputContextTest.h"
#include "Draw/Renderer.h"

ska::GameEventDispatcher& GetGED();
ska::RawInputListener& GetRawInputListener();
InputContextTest* GetICT();
ska::InputContextManager& GetICM();
void GetWindow();

struct MockRenderer : public ska::Renderer {
	SDL_Texture* createTextureFromSurface(const ska::SDLSurface& surface) const override {}

protected:
	void render(ska::SDLTexture& t, int posX, int posY, ska::Rectangle const*) const override {}
	void render(ska::GifTexture& t, int posX, int posY, ska::Rectangle const*) const override {}
};

MockRenderer MakeMockRenderer();