#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

#include <algorithm>
#include <string>
#include <utility>
#include <optional>

#include "../../Exceptions/InputException.h"

//Windows only
#define NOMINMAX
#include <windows.h>

namespace ska {
	class DynamicLibraryWindows {
	public:
		DynamicLibraryWindows(const std::string& lib) {
			const auto errorMessage = loadLibrary((lib + ".dll").c_str());
			if (m_handle == nullptr) {
				throw ska::InputException(std::move(errorMessage));
			}
		}

		~DynamicLibraryWindows() {
			if (m_handle != nullptr) {
				FreeLibrary(m_handle);
			}
		}

		std::pair<void*, std::string> getFunction(const char* name) const {
			char errbuf[512];

			auto function = static_cast<void *>(GetProcAddress(m_handle, name));
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
	};
}
#endif
