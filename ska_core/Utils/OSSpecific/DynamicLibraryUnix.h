#pragma once
#include "../SkaConstants.h"
#include "../../Logging/Logger.h"
#if defined(SKA_PLATFORM_LINUX)
#include <optional>
#include <string>
#include "../../Exceptions/InputException.h"

//Unix only
#include <dlfcn.h>

namespace ska {
	class DynamicLibraryUnix {
	public:
		DynamicLibraryUnix(const std::string& lib) : 
			m_name("lib" + lib + ".so") {
			auto errorMessage = loadLibrary(m_name.c_str());
			if(m_handle == nullptr) {
				ExceptionTrigger<InputException>(errorMessage, ExceptionAbort);
			}
		}

		~DynamicLibraryUnix() {
			if(m_handle != nullptr) {
				SKA_LOG_INFO("Unloading dynamic library ", m_name);
				dlclose(m_handle);
			}
		}

		std::pair<void*, std::string> getFunction(const char* name) const {
			auto function = dlsym(m_handle, name);
			return std::make_pair(function, (function == nullptr ? std::string((char *)dlerror()) : ""));
		}

	private:
		const char* loadLibrary(const char* lib) {
			SKA_LOG_INFO("Loading dynamic library ", lib);
			m_handle = dlopen(lib, RTLD_NOW);
			if (m_handle == nullptr) {
				return dlerror();
			}
			return "";
		}

		void* m_handle {};
		std::string m_name;
	};
}
#endif
