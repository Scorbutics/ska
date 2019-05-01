#pragma once
#include "InputException.h"

namespace ska {
	class InvalidPathException : public InputException {
	public:

		InvalidPathException(const char* message) : InputException(message, "InvalidPathException") {
		}

		virtual ~InvalidPathException() {
		}
	};
}
