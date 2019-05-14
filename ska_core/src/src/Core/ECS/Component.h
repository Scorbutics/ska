#pragma once
#include <cstddef>

namespace ska {
    template <class DerivedComponentType>
    struct Component {
        static inline std::size_t TYPE_ID;
        static inline const char* TYPE_NAME;
    };
}
#define SKA_DECLARE_COMPONENT(ComponentName) template<> std::size_t ska::Component<ComponentName>::TYPE_ID = static_cast<std::size_t>(-1);\
template<> const char* ska::Component<ComponentName>::TYPE_NAME = #ComponentName;
