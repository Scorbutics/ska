#pragma once
#include "Utils/DynamicLibrary.h"
#include <gsl/pointers>
#include <SDL.h>
#include "CEV_gif.h"

#define SKA_GIF_LIB_CALLS_DEFINE(ENUM, FUNCTION, NAME) \
			SKA_LIB_CALLS_DEFINE(SDLGifIdNamedFunction, ENUM, FUNCTION, NAME)

namespace ska {

	template<int Id>
	struct SDLGifIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum SDLGifCalls {
		GIF_TEXTURE,
		GIF_FRAME_NEXT,
		GIF_TIME_SET,
		GIF_LOOP_MODE,
		GIF_LOOP_RESET,
		GIF_ANIM_AUTO,
		GIF_ANIM_FREE
	};

	
	SKA_GIF_LIB_CALLS_DEFINE(GIF_TEXTURE, SDL_Texture* (CEV_GifAnim*), "CEV_gifTexture");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_FRAME_NEXT, void(CEV_GifAnim*), "CEV_gifFrameNext");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_TIME_SET, void(CEV_GifAnim*, unsigned int, uint16_t), "CEV_gifTimeSet");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_LOOP_MODE, void(CEV_GifAnim*, unsigned int), "CEV_gifLoopMode");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_LOOP_RESET, void(CEV_GifAnim*), "CEV_gifLoopReset");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_ANIM_AUTO, void(CEV_GifAnim*), "CEV_gifAnimAuto");
	SKA_GIF_LIB_CALLS_DEFINE(GIF_ANIM_FREE, void(CEV_GifAnim*), "CEV_gifAnimFree");

	using SDLGifDynLib = DynamicLibrary <
		SDLGifIdNamedFunction<GIF_TEXTURE>,
		SDLGifIdNamedFunction<GIF_FRAME_NEXT>,
		SDLGifIdNamedFunction<GIF_TIME_SET>,
		SDLGifIdNamedFunction<GIF_LOOP_MODE>,
		SDLGifIdNamedFunction<GIF_LOOP_RESET>,
		SDLGifIdNamedFunction<GIF_ANIM_AUTO>,
		SDLGifIdNamedFunction<GIF_ANIM_FREE>
	>;

	class SDLGifLibrary : public SDLGifDynLib {
#define callSDLGif(enumIndex) call<SDLGifIdNamedFunction<enumIndex>>
	public:

		SDL_Texture& getTexture(CEV_GifAnim& anim) const {
			return *callSDLGif(GIF_TEXTURE)(&anim);
		}

		void frameNext(CEV_GifAnim& anim) const {
			callSDLGif(GIF_FRAME_NEXT)(&anim);
		}

		void timeSet(CEV_GifAnim& anim, unsigned int index, uint16_t timeMs) const {
			callSDLGif(GIF_TIME_SET)(&anim, index, timeMs);
		}

		void loopMode(CEV_GifAnim& anim, unsigned int loopMode) const {
			callSDLGif(GIF_LOOP_MODE)(&anim, loopMode);
		}

		void loopReset(CEV_GifAnim& anim) const {
			callSDLGif(GIF_LOOP_RESET)(&anim);
		}

		void animAuto(CEV_GifAnim& anim) const {
			callSDLGif(GIF_ANIM_AUTO)(&anim);
		}

		void animFree(CEV_GifAnim* anim) const {
			callSDLGif(GIF_ANIM_FREE)(anim);
		}
		
		static const SDLGifLibrary& get() {
			static SDLGifLibrary instance;
			return instance;
		}
		#undef callSDLImage
	private:
		SDLGifLibrary() : SDLGifDynLib("SDL2_gif") {}
	};
}
