#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

#include <string>
#include <utility>

//Windows only
#undef NOMINMAX
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

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
			//On Windows, pointers to non-member functions and pointers to objects are the same size
			//So we can cast from FARPROC to void* without problem
			auto function = reinterpret_cast<void *>(GetProcAddress(m_handle, name));
			if (function == NULL) {
				return std::make_pair(nullptr, GenerateLastErrorMessage());
			}
			return std::make_pair(function, "");
		}

		const std::string& errorMessage() const { return m_errorMessage; }

	private:
		std::string loadLibrary(const char* lib) {
			m_handle = LoadLibraryA(lib);

			/* Generate an error message if all loads failed */
			if (m_handle == NULL) {
				return GenerateLastErrorMessage();
			}
			return "";
		}

		static std::string GenerateLastErrorMessage() {
			char errBuf[512];
			FormatMessage((FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM),
				NULL, GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				errBuf, sizeof(errBuf), NULL);
			// Trim trailing spaces
			char* end = errBuf + strlen(errBuf) - 1;
			while (end > errBuf && isspace((unsigned char)* end)) end--;
			end[1] = '\0';

			return std::string(errBuf);
		}

		HMODULE m_handle{};
		std::string m_errorMessage;
	};
}
#endif
