#pragma once
#include <string>
#include "GenericException.h"

namespace ska {
	class IllegalArgumentException : public GenericException {
	public:

		IllegalArgumentException(const char* message) : IllegalArgumentException(message, "IllegalArgumentException") {
		}

		IllegalArgumentException(const char* message, const char* type) : GenericException(message, type) {
		}

		virtual ~IllegalArgumentException() throw() {
		}
	};
}
