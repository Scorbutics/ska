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
		static std::size_t id() { return m_id; }
		static void setId(std::size_t id) { m_id = id; }
	private:
		static size_t m_id;
	};
}
