#ifndef NDEBUG
#define SKA_DEBUG_GRAPHIC
#define SKA_DEBUG_LOGS
#endif

// The web (Emscripten, WebAssembly) : a POSIX platform (SKA_PLATFORM_LINUX) without dynamic libraries
#if defined(__EMSCRIPTEN__)
#define SKA_PLATFORM_WEB
#endif

#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux) || defined(__EMSCRIPTEN__)
#define SKA_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#define SKA_PLATFORM_WIN
#else
#define SKA_PLATFORM_UNKNOWN
#endif

#if defined(__cpp_exceptions) && __cpp_exceptions < 199711L 
#define SKA_EXCEPTIONS_DISABLED
#endif
