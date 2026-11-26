#pragma once
#include <functional>
#include <iostream>

namespace ska {
    namespace bytecode {
        using DeserializationStrategy = std::function<std::istream&(const std::string&)>;
        using SerializationStrategy = std::function<std::ostream&(const std::string&)>;

        struct DeserializationStrategyType {
            static DeserializationStrategy PerScript();
        };

        struct SerializationStrategyType {
            static SerializationStrategy PerScript();
        };
    }
}
