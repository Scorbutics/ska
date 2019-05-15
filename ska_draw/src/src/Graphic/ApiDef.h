#pragma once

#ifdef ska_draw_EXPORTS
#undef SKA_COMPONENT_API
#define SKA_COMPONENT_API __declspec(dllexport)
#else 
#undef SKA_COMPONENT_API
#define SKA_COMPONENT_API __declspec(dllimport)
#endif
