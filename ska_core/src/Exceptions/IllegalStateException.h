#pragma once
#include "GenericException.h"

namespace ska {
	class IllegalStateException : public GenericException {
	public:

		IllegalStateException(const char* message) : IllegalStateException(message, "IllegalStateException") {
		}

		IllegalStateException(const char* message, const char* type) : GenericException(message, type) {
		}

		virtual ~IllegalStateException() {
		}
	};
}
