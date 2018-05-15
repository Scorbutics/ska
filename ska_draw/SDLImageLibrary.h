#pragma once
#include "Utils/DynamicLibrary.h"

namespace ska {

	template<int Id>
	struct SDLImageIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum SDLImageCalls {
		IMG_INIT,
		IMG_QUIT,
		IMG_LOAD
	};

	using SDLImageDynLib = DynamicLibrary <
		SDLImageIdNamedFunction<IMG_INIT>,
		SDLImageIdNamedFunction<IMG_QUIT>,
		SDLImageIdNamedFunction<IMG_LOAD>>;
	

	class SDLImageLibrary : public SDLImageDynLib {
	public:
		int init() const {
			//return call<SDLImageIdNamedFunction<IMG_INIT>>();
			return 0;
		}

		static const SDLImageLibrary& get() {
			static SDLImageLibrary instance;
			return instance;
		}

	private:
		SDLImageLibrary() : SDLImageDynLib("SDL") {}
	};
}
