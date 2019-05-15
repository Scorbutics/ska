#pragma once
#include <cstddef>
#include <string>

namespace ska {
	std::string ComponentNameExtract(const char* componentName);

    template <class DerivedComponentType>
	struct Component final {
        static std::size_t& TYPE_ID();
		static const std::string& TYPE_NAME();
    };
}

#define SKA_DEFINE_COMPONENT(ComponentName) \
template<> std::size_t& ska::Component<ComponentName>::TYPE_ID() { static std::size_t id = static_cast<std::size_t>(-1); return id; }\
template<> const std::string& ska::Component<ComponentName>::TYPE_NAME() { static std::string componentName = ComponentNameExtract(#ComponentName); return componentName; }


#define SKA_DECLARE_COMPONENT(ComponentName) \
template<> std::size_t& ska::Component<ComponentName>::TYPE_ID();\
template<> const std::string& ska::Component<ComponentName>::TYPE_NAME();
