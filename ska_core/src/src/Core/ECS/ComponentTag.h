#pragma once
#include <cstddef>
#ifdef SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_API __declspec(dllexport)
#else
#define SKA_CORE_API __declspec(dllimport)
#endif

namespace ska {
	template <typename T>
	class ComponentTag {
	public:
		static size_t m_id;
	};
}
