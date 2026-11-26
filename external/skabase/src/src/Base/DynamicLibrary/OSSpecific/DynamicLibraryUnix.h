#pragma once
#include "../../SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)
#include <optional>
#include <string>

//Unix only
#include <dlfcn.h>

namespace ska {
	class DynamicLibraryUnix {
	public:
		DynamicLibraryUnix(const std::string& lib) : 
			m_name("lib" + lib + ".so") {
			m_errorMessage = loadLibrary(m_name.c_str());
			if(!isLoaded()) {
				SLOG_STATIC(LogLevel::Warn, DynamicLibraryTag) << m_errorMessage;
			}
		}

		bool isLoaded() const {
			return m_handle != nullptr;
		}

		~DynamicLibraryUnix() {
			if(m_handle != nullptr) {
				SLOG_STATIC(LogLevel::Info, DynamicLibraryTag) << "Unloading dynamic library " << m_name;
				dlclose(m_handle);
			}
		}

		std::pair<ska_default_function_ptr, std::string> getFunction(const char* name) const {
			auto function = reinterpret_cast<ska_default_function_ptr>(dlsym(m_handle, name));
			return std::make_pair(function, (function == nullptr ? std::string((char *)dlerror()) : ""));
		}


		const std::string& errorMessage() const { return m_errorMessage; }

	private:
		const char* loadLibrary(const char* lib) {
			SLOG_STATIC(LogLevel::Info, DynamicLibraryTag) << "Loading dynamic library " << lib;
			m_handle = dlopen(lib, RTLD_NOW);
			if (m_handle == nullptr) {
				return dlerror();
			}
			return "";
		}

		void* m_handle {};
		std::string m_name;
		std::string m_errorMessage;
    };
}
#endif
