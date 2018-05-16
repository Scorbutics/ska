#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

#include <string>
#include <utility>

//Windows only
#define NOMINMAX
#include <windows.h>

namespace ska {
	class DynamicLibraryWindows {
	public:
		DynamicLibraryWindows(const std::string& lib) {
			m_errorMessage = loadLibrary((lib + ".dll").c_str());
		}

		bool isLoaded() const {
			return m_handle != nullptr;
		}

		~DynamicLibraryWindows() {
			if (m_handle != nullptr) {
				FreeLibrary(m_handle);
			}
		}

		std::pair<void*, std::string> getFunction(const char* name) const {
			char errbuf[512];

			//On Windows, pointers to non-member functions and pointers to objects are the same size
			//So we can cast from FARPROC to void* without problem
			auto function = reinterpret_cast<void *>(GetProcAddress(m_handle, name));
			if (function == NULL) {
				FormatMessage((FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_FROM_SYSTEM),
					NULL, GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					errbuf, sizeof(errbuf), NULL);
				return std::make_pair(nullptr, errbuf);
			}
			return std::make_pair(function, "");
		}

	private:
		std::string loadLibrary(const char* lib) {
			char errbuf[512];

			m_handle = LoadLibraryA(lib);

			/* Generate an error message if all loads failed */
			if (m_handle == NULL) {
				FormatMessageA((FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_FROM_SYSTEM),
					NULL, GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					errbuf, sizeof(errbuf), NULL);
				return std::string(errbuf);
			}
			return "";
		}

		HMODULE m_handle{};
		std::string m_errorMessage;
	};
}
#endif
