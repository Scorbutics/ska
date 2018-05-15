#include <doctest.h>
#include <SDL.h>
#include <iostream>
#include "Exceptions/ExceptionTrigger.h"
#include "Utils/DynamicLibrary.h"
#include "Utils/StringConstExpr.h"

namespace dynlib {

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

	enum CCalls {
		C_STRLEN = SDL_INIT + 1,
		C_PRINTF
	};

	template<>
	std::string IdNamedFunction<CCalls::C_STRLEN>::name = "strlen";

	template<>
	std::string IdNamedFunction<CCalls::C_PRINTF>::name = "printf";

	using CDynLib = ska::DynamicLibrary<
		IdNamedFunction<CCalls::C_STRLEN>,
		IdNamedFunction<CCalls::C_PRINTF>
	      >;
}

class CLibrary : public dynlib::CDynLib {
public:
	CLibrary() : dynlib::CDynLib("c") {}

	int len(const std::string& str) {
		return call<dynlib::IdNamedFunction<dynlib::CCalls::C_STRLEN>, int(const char*)>(str.c_str());
	}

	template <class ... Str>
	int print(const char* format, Str&& ... str) {
		return call<dynlib::IdNamedFunction<dynlib::CCalls::C_PRINTF>, int(const char*, Str...)>(std::move(format), std::forward<Str>(str)...);
	}
};

class SDLLibrary : public dynlib::SDLDynLib {
public:
	SDLLibrary() : dynlib::SDLDynLib("SDL") {}

	int init(Uint32 flags) {
		return call<dynlib::IdNamedFunction<dynlib::SDLCalls::SDL_INIT>, int(Uint32)>(std::move(flags));
	}

	void logCritical(int category, const std::string& mess) {
		call<dynlib::IdNamedFunction<dynlib::SDLCalls::SDL_LOG_CRITICAL>, void(int, const char*)>(std::move(category), mess.c_str());
	}
};

TEST_CASE("[DynamicLibrary] Loading failure") {
	auto sdlLib = SDLLibrary{};
	CHECK(!sdlLib.isLoaded());
}

#ifdef SKA_PLATFORM_LINUX
//Conditional TUs : cannot test "libc.so" on Windows


TEST_CASE("[DynamicLibrary] Loading") {
	auto clib = std::optional<CLibrary>{};
	clib = CLibrary{};
	CHECK(clib->isLoaded());

	SUBCASE("calling strlen") {
		CHECK(clib->hasLoaded<dynlib::IdNamedFunction<dynlib::CCalls::C_STRLEN>>());
		CHECK(4 == clib->len("Test"));
	}

	SUBCASE("calling a varargs function : printf") {
		CHECK(clib->hasLoaded<dynlib::IdNamedFunction<dynlib::CCalls::C_PRINTF>>());
		clib->print("%s LOL %i\n", "test !", 45);
	}
}

#endif

