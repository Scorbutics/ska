#ifndef NDEBUG
#define SKA_DEBUG_GRAPHIC
#define SKA_DEBUG_LOGS
#endif

#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
#define SKA_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#define SKA_PLATFORM_WIN
#else
#define SKA_PLATFORM_UNKNOWN
#endif

#if defined(__cpp_exceptions) && __cpp_exceptions < 199711L 
#define SKA_EXCEPTIONS_DISABLED
#endif

/* DEBUG CONF */
#undef SKA_DEBUG_GRAPHIC
/* END DEBUG CONF*/

//#define FILE_SEPARATOR "/"
#define NOSUCHFILE "./Sprites/nosuchfile.png"
#define SPRITEBANK_ANIMATION 0
#define SPRITEBANK_SKILL 1
#define SPRITEBANK_PHYSIC 2
#define SPRITEBANK_FACESET 3
#define SPRITEBANK_CHARSET 4
#define SPRITEBANK_PARTICLE 5
#define SPRITEBANK_ICONS 6
#define SPRITEBANK_INVENTORY 7
#define SPRITEBANK_NUMBER 8

#define SPRITEBANK_PATH_DIR "./Sprites/"
#define SPRITEBANK_PATH_ANIMATION SPRITEBANK_PATH_DIR "Animations"
#define SPRITEBANK_PATH_SKILL SPRITEBANK_PATH_DIR "Fight"
#define SPRITEBANK_PATH_PHYSIC SPRITEBANK_PATH_DIR "Physics"
#define SPRITEBANK_PATH_FACESET SPRITEBANK_PATH_DIR "Facesets"
#define SPRITEBANK_PATH_CHARSET SPRITEBANK_PATH_DIR "Charsets"
#define SPRITEBANK_PATH_PARTICLE SPRITEBANK_PATH_DIR "Particles"
#define SPRITEBANK_PATH_ICONS SPRITEBANK_PATH_DIR "Icones"
#define SPRITEBANK_PATH_INVENTORY SPRITEBANK_PATH_DIR "Inventory"
