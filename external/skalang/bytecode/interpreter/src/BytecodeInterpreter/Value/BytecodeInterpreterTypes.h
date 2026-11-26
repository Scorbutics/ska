#pragma once
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Value/VectorMapped.h"
#include "Runtime/Value/InterpreterTypes.h"

namespace ska {
	namespace bytecode {
    	class Script;
    	template <class T>
    	using SharedDeque = std::shared_ptr<std::deque<T>>;
	}

	template <>
	struct InterpreterTypes<ska::bytecode::Interpreter> {
    	using Script = ska::bytecode::Script;
    	using ScriptCache = ska::bytecode::ScriptCache;
    	using Memory = void;
	};
}
