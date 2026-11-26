#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeGenerationContext.h"
#include "Value/BytecodeGenerationOutput.h"
#include "Value/BytecodeScriptGenerationHelper.h"
#include "Value/BytecodeScriptGeneration.h"
#include "Value/BytecodeScriptCache.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;
	class ASTNode;

	namespace bytecode {
		class GeneratorOperatorBase;

		class Generator {
			using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
		public:
			Generator(const ReservedKeywordsPool& reserved);
			~Generator() = default;

			const ScriptGeneration& generate(ScriptCache& cache, ScriptGenerationHelper script);
			const ScriptGeneration& generate(ScriptCache& cache, std::size_t scriptIndex);
			InstructionOutput generatePart(GenerationContext node);

		private:
			OperatorGenerator build();

			OperatorGenerator m_operatorGenerator;
		};
	}
}
