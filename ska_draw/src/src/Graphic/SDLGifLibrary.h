#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <gsl/pointers>

struct SDL_Texture;
struct CEV_GifAnim;

namespace ska {

	template<int Id>
	struct SDLGifIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum class SDLGifCalls {
		GIF_TEXTURE,
		GIF_FRAME_NEXT,
		GIF_TIME_SET,
		GIF_LOOP_MODE,
		GIF_LOOP_RESET,
		GIF_ANIM_AUTO,
		GIF_ANIM_FREE
	};

#define SKA_GIF_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(SDLGifIdNamedFunction, static_cast<int>(SDLGifCalls::ENUM), FUNCTION)

	SKA_GIF_LIB_CALLS_DEFINE(GIF_TEXTURE, SDL_Texture* (CEV_GifAnim*));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_FRAME_NEXT, void(CEV_GifAnim*));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_TIME_SET, void(CEV_GifAnim*, unsigned int, uint16_t));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_LOOP_MODE, void(CEV_GifAnim*, unsigned int));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_LOOP_RESET, void(CEV_GifAnim*));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_ANIM_AUTO, void(CEV_GifAnim*));
	SKA_GIF_LIB_CALLS_DEFINE(GIF_ANIM_FREE, void(CEV_GifAnim*));

#define SKA_GIF_DYN_LIB_NAME_ENTRY(ENUM) SDLGifIdNamedFunction<static_cast<int>(SDLGifCalls::ENUM)>

	using SDLGifDynLib = DynamicLibrary <
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_TEXTURE),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_FRAME_NEXT),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_TIME_SET),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_LOOP_MODE),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_LOOP_RESET),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_ANIM_AUTO),
		SKA_GIF_DYN_LIB_NAME_ENTRY(GIF_ANIM_FREE)
	>;

	class SDLGifLibrary : public SDLGifDynLib {
#define callSDLGif(enumIndex) call<SKA_GIF_DYN_LIB_NAME_ENTRY(enumIndex)>
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
		
		static const SDLGifLibrary& get();

		#undef callSDLImage
	private:
		SDLGifLibrary() : SDLGifDynLib("SDL2_gif") {}
	};
}
