#pragma once
#include <cstddef>

namespace ska {
    template <class DerivedComponentType>
	struct SKA_COMPONENT_API Component final {
        static std::size_t TYPE_ID;
		static const char* TYPE_NAME;
    };
}

#define SKA_DEFINE_COMPONENT(ComponentName) \
template struct ska::Component<ComponentName>;\
template<> std::size_t ska::Component<ComponentName>::TYPE_ID = static_cast<std::size_t>(-1);\
template<> const char* ska::Component<ComponentName>::TYPE_NAME = #ComponentName;

#ifndef SKA_IN_COMPONENT
	#define SKA_DECLARE_COMPONENT(ComponentName) extern template struct ska::Component<ComponentName>;
#else
	#define SKA_DECLARE_COMPONENT(ComponentName) 
#endif
