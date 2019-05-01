#pragma once
#include "GenericException.h"
namespace ska {
	class NumberFormatException : public GenericException {
	public:
		explicit NumberFormatException(const char* message) : GenericException(message, "NumberFormatException") {
		}

		virtual ~NumberFormatException() {
		}
	};
}
