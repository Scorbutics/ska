#include <doctest.h>
#include <SDL.h>
#include <iostream>
#include "Utils/DynamicLibrary.h"
#include "Utils/StringConstExpr.h"

namespace sdldynlib {

	template<int Id>
	struct IdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};
	
	enum SDLCalls {
		SDL_LOG_CRITICAL,
		SDL_INIT
	};
	
	template<>
	std::string IdNamedFunction<SDLCalls::SDL_LOG_CRITICAL>::name = "SDL_LogCritical";
	
	template<>
	std::string IdNamedFunction<SDLCalls::SDL_INIT>::name = "SDL_Init";


	using SDLDynLib = ska::DynamicLibrary<IdNamedFunction<SDLCalls::SDL_LOG_CRITICAL>, IdNamedFunction<SDLCalls::SDL_INIT>>;
}

class SDLLibrary : public sdldynlib::SDLDynLib {

public:
	SDLLibrary() : 
		sdldynlib::SDLDynLib("libc++") {
	}

	int init(Uint32 flags) {
		return call<sdldynlib::IdNamedFunction<sdldynlib::SDLCalls::SDL_INIT>, int(Uint32)>(std::move(flags));
	}

	void logCritical(int category, const std::string& mess) {
		call < sdldynlib::IdNamedFunction<sdldynlib::SDLCalls::SDL_LOG_CRITICAL>, void(int, const char*) > (std::move(category), mess.c_str());
	}
};

TEST_CASE("[DynamicLibrary]") {
	
		auto sdlLib = SDLLibrary{};
		sdlLib.init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		sdlLib.logCritical(SDL_LOG_CATEGORY_AUDIO, "toto");
	
		CHECK(true);
	
}
