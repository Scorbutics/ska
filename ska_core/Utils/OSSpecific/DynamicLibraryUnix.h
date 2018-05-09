#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)
#include <optional>
#include <string>
#include "../../Exceptions/InputException.h"

//Unix only
#include <dlfcn.h>

namespace ska {
	class DynamicLibraryUnix {
	public:
		DynamicLibraryUnix(const char* lib) {
			auto errorMessage = loadLibrary(lib);
			if(m_handle == nullptr) {
				throw ska::InputException(std::move(errorMessage));
			}
		}

		~DynamicLibraryUnix() {
			if(m_handle != nullptr) {
				dlclose(m_handle);
			}
		}

		std::pair<void*, std::string> getFunction(const char* name) const {
			auto function = dlsym(m_handle, name);
			return std::make_pair(function, (function == nullptr ? std::string((char *)dlerror()) : ""));
		}

	private:
		std::string loadLibrary(const char* lib) {
			m_handle = dlopen(lib, RTLD_NOW);
			if (m_handle == nullptr) {
				return std::string(dlerror());
			}
			return "";
		}

		void* m_handle {};
	};
}
#endif
