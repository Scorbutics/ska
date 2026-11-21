#pragma once

#include <deque>
#include <string>
#include "Token.h"

namespace ska {
    enum class ParsingContextType {
        // The value of a variable is a function declaration : the function takes the name of the variable
        AFFECTATION,
        // Any other value : the functions declared in it are anonymous
        VALUE,
        FUNCTION_DECLARATION,
        FACTORY_DECLARATION,
        FUNCTION_MEMBER_DECLARATION
    };

    struct ParsingContext {
        ParsingContextType type;
        Token owner;
    };

    using ParsingContextStack = std::deque<ParsingContext>;

    // Name of an anonymous function (declared as a call argument, an array element...) : no script can use it
    inline std::string AnonymousFunctionName(const Cursor& position) {
        return "#function_" + std::to_string(position.line) + "_" + std::to_string(position.column);
    }

    inline bool IsAnonymousFunctionName(const std::string& name) {
        return !name.empty() && name[0] == '#';
    }
}
