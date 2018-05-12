#pragma once
#include "GenericException.h"

namespace ska {
	class InputException : public GenericException {
	public:

		InputException(const char* message) : GenericException(message, "InputException") {
		}

		InputException(const char* message, const char* type) : GenericException(message, type) {
		}

		virtual ~InputException() {
		}
	};

}
