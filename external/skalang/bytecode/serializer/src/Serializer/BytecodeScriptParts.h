#pragma once
#include <cstddef>

#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "BytecodeScriptExternalReferences.h"

namespace ska {
	namespace bytecode {
		struct ScriptParts {
			ScriptParts(SerializerNativeContainer& natives) : body(natives), header(natives), references(natives) {}

			ScriptHeader header;
			ScriptBody body;
			ScriptExternalReferences references;
		};
	}
}
