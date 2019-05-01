#pragma once
#include "GenericException.h"

namespace ska {
	class TerminateProcessException : public GenericException {
	public:

		TerminateProcessException(const char* message) : TerminateProcessException(message, "TerminateProcessException") {
		}

		TerminateProcessException(const char* message, const char* type) : GenericException(message, type) {
		}

		virtual ~TerminateProcessException() {
		}
	};
}
