#pragma once

#ifndef NDEBUG
#ifndef SKA_DEBUG_ONLY
#define SKA_DEBUG_ONLY(x) do { x } while (0)
#endif
#else
#ifndef SKA_DEBUG_ONLY
#define SKA_DEBUG_ONLY(x) 
#endif
#endif
