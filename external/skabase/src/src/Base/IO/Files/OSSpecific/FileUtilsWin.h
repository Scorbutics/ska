#pragma once
#include "Base/SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

namespace ska {
	class FileUtilsWin {
	private:
		FileUtilsWin();

	public:
		~FileUtilsWin();
		static std::string getCurrentDirectory();
		static std::string getExecutablePath();
		static void createDirectory(const std::string& directoryName);
		static bool isAbsolutePath(const std::string& path);
		static std::string getCanonicalPath(const std::string& path);
	};
}
#endif
