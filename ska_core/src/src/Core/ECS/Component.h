#pragma once
#include <cstddef>

namespace ska {
    template <class DerivedComponentType>
	struct Component final {
        static std::size_t& TYPE_ID();
		static const char* TYPE_NAME();
    };
}

#define SKA_DEFINE_COMPONENT(ComponentName) \
template<> std::size_t& ska::Component<ComponentName>::TYPE_ID() { static std::size_t id = static_cast<std::size_t>(-1); return id; }\
template<> const char* ska::Component<ComponentName>::TYPE_NAME() { return #ComponentName; }


#define SKA_DECLARE_COMPONENT(ComponentName) \
template<> std::size_t& ska::Component<ComponentName>::TYPE_ID();\
template<> const char* ska::Component<ComponentName>::TYPE_NAME();
