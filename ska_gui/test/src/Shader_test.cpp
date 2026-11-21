#include <doctest.h>
#include <SDL3/SDL.h>
#include "LoggerConfig.h"
#include "Core/SDLLibrary.h"
#include "Graphic/SDLWindow.h"
#include "Graphic/SDLRenderer.h"
#include "Graphic/Shader.h"
#include "Graphic/Texture.h"

namespace {
	// The color of a pixel of the current target of the renderer
	ska::Color ReadPixel(ska::SDLRenderer& renderer, int x, int y) {
		const auto& sdl = ska::SDLLibrary::get();
		const auto rect = SDL_Rect{ x, y, 1, 1 };
		auto* surface = sdl.readPixels(*renderer.unwrap(), &rect);
		auto color = ska::Color{};
		if (surface != nullptr) {
			sdl.readPixel(*surface, 0, 0, &color.r, &color.g, &color.b, &color.a);
			sdl.freeSurface(surface);
		}
		return color;
	}

	// Draws a white square in a target texture, with an effect, and gives the color of its middle
	ska::Color DrawWhiteSquare(ska::SDLRenderer& renderer, const ska::RenderEffect* effect) {
		auto target = ska::Texture{};
		target.loadAsTarget(renderer, 32, 32);
		auto square = ska::Texture{};
		square.loadFromColoredRect(16, 16, ska::Color{ 255, 255, 255, 255 });

		renderer.setRenderTarget(&target);
		renderer.setEffect(effect);
		renderer.render(square, 8, 8, nullptr, 0., nullptr);
		renderer.setEffect(nullptr);
		const auto color = ReadPixel(renderer, 16, 16);
		renderer.setRenderTarget(nullptr);
		return color;
	}
}

TEST_CASE("[Shader] a fragment shader runs on a drawing") {
	auto window = ska::SDLWindow{ "shader test", 480, 416 };
	window.load();
	auto renderer = ska::SDLRenderer{ window };
	renderer.load();
	if (ska::SDLLibrary::get().gpuRendererDevice(*renderer.unwrap()) == nullptr) {
		MESSAGE("No \"gpu\" renderer here : the shaders cannot be tested");
		return;
	}

	SUBCASE("Without shader, the square is white") {
		const auto color = DrawWhiteSquare(renderer, nullptr);
		CHECK(color.r == 255);
		CHECK(color.g == 255);
		CHECK(color.b == 255);
	}

	SUBCASE("The shader inverts the colors : the square is black") {
		auto invert = ska::Shader{ SKA_TEST_SHADERS_DIR "/invert" };
		auto effect = ska::RenderEffect{};
		effect.shader = &invert;
		const auto color = DrawWhiteSquare(renderer, &effect);
		CHECK(color.r == 0);
		CHECK(color.g == 0);
		CHECK(color.b == 0);
	}

	SUBCASE("The uniforms are given to the shader") {
		auto uniformColor = ska::Shader{ SKA_TEST_SHADERS_DIR "/uniform_color", true };
		const float red[4] = { 1.F, 0.F, 0.F, 1.F };
		uniformColor.setUniforms(red);
		auto effect = ska::RenderEffect{};
		effect.shader = &uniformColor;
		const auto color = DrawWhiteSquare(renderer, &effect);
		CHECK(color.r == 255);
		CHECK(color.g == 0);
		CHECK(color.b == 0);
	}

	SUBCASE("A shader that cannot be read : the drawing is done without it") {
		auto missing = ska::Shader{ SKA_TEST_SHADERS_DIR "/missing" };
		auto effect = ska::RenderEffect{};
		effect.shader = &missing;
		const auto color = DrawWhiteSquare(renderer, &effect);
		CHECK(color.r == 255);
	}
}
