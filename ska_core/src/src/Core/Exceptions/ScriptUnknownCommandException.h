#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptUnknownCommandException : public ScriptException {
	public:

		ScriptUnknownCommandException(std::string message) : ScriptException(message.c_str(), "ScriptUnknownCommandException") {
		}

		virtual ~ScriptUnknownCommandException() {
		}
	};
}
