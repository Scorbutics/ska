#pragma once
#include <vector>
#include <string>
#include "Generator/BytecodeCommand.h"

namespace ska {
  namespace bytecode {
	class ScriptGeneration;

	struct BytecodePart {
  	Command command;
  	std::string dest;
  	std::string left;
  	std::string right;
	};
  }
}

void BytecodeCompare(const ska::bytecode::ScriptGeneration& ScriptGeneration, std::vector<ska::bytecode::BytecodePart> expected);
