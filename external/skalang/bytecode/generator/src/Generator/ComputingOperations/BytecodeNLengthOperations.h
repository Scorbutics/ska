#pragma once
#include <limits>
#include "Generator/BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/BytecodeGenerationContext.h"
#include "Generator/BytecodeGenerator.h"

namespace ska {
  namespace bytecode {
	template <Command command, class NodeIterator, size_t Modulo = 3>
		std::size_t ApplyNOperations(Generator& generator, InstructionOutput& output, GenerationContext& script, NodeIterator start, NodeIterator end) {
			auto temporaryContainer = std::vector<OperandUse> {};

			std::size_t index = 0u;
			for (auto paramNodeIt = start; paramNodeIt != end; paramNodeIt++) {
				const auto& paramNode = *paramNodeIt;
				if (paramNode != nullptr) {
					auto finalGroup = generator.generatePart(script.next(*paramNode));
					temporaryContainer.push_back(finalGroup.operand());
					output.push(std::move(finalGroup));
					if (temporaryContainer.size() == Modulo) {
						output.push(Instruction { command, std::move(temporaryContainer)});
						temporaryContainer = {};
					}
				}
				index++;
			}
			assert(temporaryContainer.size() <= Modulo);
			if (!temporaryContainer.empty()) {
				output.push(Instruction{ command, std::move(temporaryContainer)});
			}
			return index;
		}
  }
}
