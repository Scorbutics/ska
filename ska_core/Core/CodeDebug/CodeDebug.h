#pragma once

#ifndef NDEBUG
#ifndef SKA_DBG_ONLY
#define SKA_DBG_ONLY(x) do { x } while (0)
#endif
#else
#ifndef SKA_DBG_ONLY
#define SKA_DBG_ONLY(x) 
#endif
#endif
