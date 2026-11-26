#pragma once
#include <string>

namespace ska {
    struct SerializerErrorData {
        std::size_t bytesRequested = 0;
        std::size_t bytesAvailable = 0;
        std::size_t bytesAllocated = 0;
        std::string zone;
        std::string error;
    };
}