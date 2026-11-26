#pragma once
#include <memory>
#include <vector>

#include "InterpreterCommandUnit.h"
#include "Generator/BytecodeCommand.h"

namespace ska {
	namespace bytecode {
	class Interpreter;
	template <Command cmd, class ... Args>
	static void InterpreterCommandDeclare(Interpreter& interpreter, std::vector<std::unique_ptr<InterpreterCommandUnit>>& target, Args&& ... args) {
	target[static_cast<std::size_t>(cmd)] = std::make_unique<InterpreterCommand<cmd>>(interpreter, std::forward<Args>(args)...);
	}
	}
}
