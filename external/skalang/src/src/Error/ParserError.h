#pragma once

#include <string>
#include <stdexcept>
#include "NodeValue/Cursor.h"

namespace ska {
	class ParserError : public std::runtime_error {
	public:
    	ParserError(std::string message, Cursor position) : 
        	std::runtime_error(
            	"(l." + std::to_string(position.line) + 
            	", c." + std::to_string(position.column) + 
            	") " + std::move(message)) {
    	}
    	
    	virtual ~ParserError() = default;
	};
}