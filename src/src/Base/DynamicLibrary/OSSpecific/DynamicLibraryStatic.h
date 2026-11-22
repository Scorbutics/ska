#pragma once
#include <cstring>
#include <string>
#include <utility>
#include "../../Functions/FunctionUtils.h"

namespace ska {
	// A function of a "dynamic library" linked in the executable, with its name
	struct StaticLibrarySymbol {
		const char* name;
		ska_default_function_ptr function;
	};

	// The functions linked in the executable, ended by { nullptr, nullptr }. Defined by the executable (ex : generated
	// by ska_web_executable, cmake/SkaWeb.cmake)
	const StaticLibrarySymbol* StaticLibrarySymbols();

	// The platforms without dynamic libraries (the web) : the libraries are linked in the executable, their functions
	// are found by name in StaticLibrarySymbols()
	class DynamicLibraryStatic {
	public:
		explicit DynamicLibraryStatic(const std::string& lib) :
			m_name(lib) {
		}

		bool isLoaded() const {
			return true;
		}

		std::pair<ska_default_function_ptr, std::string> getFunction(const char* name) const {
			for (const auto* symbol = StaticLibrarySymbols(); symbol->name != nullptr; symbol++) {
				if (std::strcmp(symbol->name, name) == 0) {
					return std::make_pair(symbol->function, std::string{});
				}
			}
			return std::make_pair(nullptr, "The function \"" + std::string{ name } + "\" of \"" + m_name + "\" is not linked in the executable");
		}

		const std::string& errorMessage() const { return m_error; }

	private:
		std::string m_name;
		std::string m_error;
	};
}
