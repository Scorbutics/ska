#pragma once

#include <string>
#include <stdexcept>

namespace ska {
	class LangError : public std::runtime_error {
	public:
    	LangError(std::string message) : 
        	std::runtime_error(std::move(message)) {}

    	LangError(std::string message, std::exception& exception) : 
        	std::runtime_error("in \"" + std::move(message) + "\"\n\t" + std::string{exception.what()}) {}
    	
    	virtual ~LangError() = default;
	};
}