#pragma once
#include "GenericException.h"

namespace ska {
	class StateDiedException : public GenericException {
	public:

		explicit StateDiedException(const char* message) : GenericException(message, "StateDiedException") {
		}

		virtual ~StateDiedException() {
		}
	
	};
}
