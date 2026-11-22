#include "Base/SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "FileUtilsUnix.h"

ska::FileUtilsUnix::FileUtilsUnix() {
}

std::string ska::FileUtilsUnix::getCurrentDirectory() {
   char cwd[2048];
   if (getcwd(cwd, sizeof(cwd)) == NULL) {
       throw std::runtime_error("Unknown error while getting current directory");
   }
   return std::string(cwd);
}

void ska::FileUtilsUnix::createDirectory(const std::string& directoryName) {
	struct stat st = { 0 };
	int status;

	if (stat(directoryName.c_str(), &st) == -1) {
		status = mkdir(directoryName.c_str(), 0700);
		if(status == -1) {
			throw std::runtime_error("Unknown error during creation of the directory " + directoryName);
		}
	}
}

std::string ska::FileUtilsUnix::getExecutablePath() {
#if defined(SKA_PLATFORM_WEB)
  // The web : a virtual file system, the files next to the executable are packed in "/bin"
  return "/bin/game";
#endif
  std::vector<char> buf(1024, 0);
  size_t size = buf.size();
  auto havePath = false;
  auto shouldContinue = true;

  do {
    ssize_t result = readlink("/proc/self/exe", &buf[0], size);
    if (result < 0) {
      shouldContinue = false;
    } else if (static_cast<size_t>(result) < size) {
      havePath = true;
      shouldContinue = false;
      size = result;
    } else {
      size *= 2;
      buf.resize(size);
      std::fill(std::begin(buf), std::end(buf), 0);
    }
  } while (shouldContinue);

  if (!havePath) {
    return "";
  }

  buf[size] = '\0';
  std::string path(&buf[0], size);
  return path;
}

bool ska::FileUtilsUnix::isAbsolutePath(const std::string& path) {
  return !path.empty() && path[0] == '/' ;
}

std::string ska::FileUtilsUnix::getCanonicalPath(const std::string& path) {
  char* mallocedPath = realpath(path.c_str(), NULL);
  try {
    auto result = mallocedPath == NULL ? path : std::string {mallocedPath};
    free(mallocedPath);
    return result;
  } catch(std::exception& e) {
    free(mallocedPath);
  }
  return path;
}

ska::FileUtilsUnix::~FileUtilsUnix() {
}


#endif
