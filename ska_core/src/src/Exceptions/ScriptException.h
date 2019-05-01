#pragma once
#include "GenericException.h"
namespace ska {
	class ScriptException : public GenericException {
	public:

		ScriptException(const char* message) : GenericException(message, "ScriptException") {
		}

		ScriptException(const char* message, const char* type) : GenericException(message, type) {
		}

		virtual ~ScriptException() throw() {
		}
	};
}
