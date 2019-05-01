#pragma once
#include "SkaConstants.h"

namespace ska {
	struct FileNameData {
		explicit FileNameData(const std::string& fullFileName) :
			FileNameData(build(fullFileName)) {
		}

		const std::string extension;
		const std::string path;
		const std::string name;

	private:
		FileNameData(const std::string& fpath, const std::string& fname, const std::string& ext) :
			extension(ext),
			path(fpath),
			name(fname) {
		}

		static FileNameData build(const std::string& fullFileName) {
			auto lastSlash = fullFileName.find_last_of('/');
			auto lastBackSlash = fullFileName.find_last_of('\\');
			const auto lastFileSeparator = (lastSlash != std::string::npos) ? (lastBackSlash != std::string::npos ? (lastBackSlash > lastSlash ? lastBackSlash : lastSlash) : lastSlash) : lastBackSlash;

			auto fileNameWithExt = lastFileSeparator == std::string::npos ? fullFileName : fullFileName.substr(lastFileSeparator + 1);
			const auto extDotPos = fileNameWithExt.find_last_of('.');
			const auto fpath = fullFileName.substr(0, lastFileSeparator);
			const auto ext = (extDotPos != std::string::npos) ? fileNameWithExt.substr(extDotPos + 1) : "";
			const auto fname = (extDotPos != std::string::npos) ? fileNameWithExt.substr(0, extDotPos) : fileNameWithExt;
			return FileNameData(fpath, fname, ext);
		}
	};

	template <class T>
	class FileUtilsTemplate {
	private:
		FileUtilsTemplate() = default;

	public:
		~FileUtilsTemplate() = default;

		static std::string getCurrentDirectory() {
			return T::getCurrentDirectory();
		}

		static std::string getExecutablePath() {
			return T::getExecutablePath();
		}

		static void createDirectory(const std::string& directoryName) {
			T::createDirectory(directoryName);
		}

		static void removeFile(const std::string& filename) {
			remove(filename.c_str());
		}

	};


}

#if defined(SKA_PLATFORM_LINUX)
#include "./OSSpecific/FileUtilsUnix.h"
namespace ska {
	using FileUtils = FileUtilsTemplate<FileUtilsUnix>;
}
#elif defined(SKA_PLATFORM_WIN)
#include "./OSSpecific/FileUtilsWin.h"
namespace ska {
	using FileUtils = FileUtilsTemplate<FileUtilsWin>;
}
#endif
