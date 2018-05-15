#pragma once
#include "FileException.h"

namespace ska {
	class CorruptedFileException : public FileException {
	public:
		CorruptedFileException(const char* message) : FileException(message, "CorruptedFileException") {
		}

		virtual ~CorruptedFileException() {
		}
	};
}
