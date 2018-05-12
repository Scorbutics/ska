#pragma once
#include "InputException.h"

namespace ska {
	class FileException : public InputException {
	public:
		FileException(const char* message) : InputException(message, "FileException") {
		}

		FileException(const char* message, const char* type) : InputException(message, type) {
		}

		virtual ~FileException() {
		}
	};
}
