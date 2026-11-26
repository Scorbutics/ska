#pragma once
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace ska {
	// Path of a log file : in the directory given by the environment variable SKA_LOG_DIR (a run of tools/ska-run), else
	// in the working directory
	inline std::string LogFilePath(const char* filename) {
		const auto* directory = std::getenv("SKA_LOG_DIR");
		if (directory == nullptr || *directory == '\0') {
			return filename;
		}
		auto path = std::string{ directory };
		if (path.back() != '/') {
			path += '/';
		}
		return path + filename;
	}

	// Opens a log file (LogFilePath). In a run of tools/ska-run (SKA_LOG_DIR), each line is written at once, and the standard
	// output too : the logs can be read while the game runs.
	inline std::ofstream OpenLogFile(const char* filename) {
		auto file = std::ofstream{ LogFilePath(filename) };
		const auto* directory = std::getenv("SKA_LOG_DIR");
		if (directory != nullptr && *directory != '\0') {
			file << std::unitbuf;
			std::cout << std::unitbuf;
		}
		return file;
	}
}
