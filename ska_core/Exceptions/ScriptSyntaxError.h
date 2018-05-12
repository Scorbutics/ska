#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptSyntaxError : public ScriptException {
	public:

		ScriptSyntaxError(const char* message) : ScriptException(message, "ScriptSyntaxError")
		{
		}

		virtual ~ScriptSyntaxError() throw() {
		}
	};

}
