/*
#include <doctest.h>
#include <SDL.h>
#include "Exceptions/ExceptionTrigger.h"
#include "Utils/DynamicLibrary.h"
#include "SDLLibrary.h"

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
std::string IdNamedFunction<SDL_LOG_CRITICAL>::name = "SDL_LogCritical";

template<>
std::string IdNamedFunction<SDL_INIT>::name = "SDL_Init";

using SDLDynLib = ska::DynamicLibrary<IdNamedFunction<SDL_LOG_CRITICAL>, IdNamedFunction<SDL_INIT>>;

enum CCalls {
	C_STRLEN = SDL_INIT + 1,
	C_PRINTF
};

template<>
std::string IdNamedFunction<C_STRLEN>::name = "strlen";

template<>
std::string IdNamedFunction<C_PRINTF>::name = "printf";

using CDynLib = ska::DynamicLibrary<
	IdNamedFunction<C_STRLEN>,
	IdNamedFunction<C_PRINTF>>;


namespace ska {
	template<>
	struct HasFunctionFormatTrait<IdNamedFunction<SDL_INIT>> {
		using FunctionFormat = int(Uint32);
	};

	template<>
	struct HasFunctionFormatTrait<IdNamedFunction<SDL_LOG_CRITICAL>> {
		using FunctionFormat = void(int, const char*);
	};

	template<>
	struct HasFunctionFormatTrait<IdNamedFunction<C_STRLEN>> {
		using FunctionFormat = int(const char*);
	};

	template<>
	struct HasFunctionFormatTrait<IdNamedFunction<C_PRINTF>> {
		using FunctionFormat = int(const char*);
	};
}

class CLibrary : public CDynLib {
public:
	CLibrary() : CDynLib("c") {}

	int len(const std::string& str) {
		return call<IdNamedFunction<C_STRLEN>>(str.c_str());
	}

	template <class ... Str>
	int print(const char* format, Str&& ... str) {
		return call<IdNamedFunction<C_PRINTF>, int(const char*, Str...)>(std::move(format), std::forward<Str>(str)...);
	}
};

class SDLLibrary : public SDLDynLib {
public:
	SDLLibrary() : SDLDynLib("SDL") {}

	int init(Uint32 flags) {
		return call<IdNamedFunction<SDL_INIT>>(std::move(flags));
	}

	void logCritical(int category, const std::string& mess) {
		call<IdNamedFunction<SDL_LOG_CRITICAL>>(std::move(category), mess.c_str());
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
*/
