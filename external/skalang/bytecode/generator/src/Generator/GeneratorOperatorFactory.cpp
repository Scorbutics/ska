#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorFactory.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>) << "%02c"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };

    // Obvisouly we pop in reverse order, because that's how a stack works (we previously pushed parameters in order in generation of Operator::FUNCTION_CALL)
	// Also, we avoid index 0, which is return type in reverse order, and the reverse-last (= first in the right order) that is dedicated to the private factory block
	// See Node::rend and Node::rbegin for details
    applyGenerator(ApplyNOperations<Command::POP, decltype(node.begin())>, result, context, node.begin(), node.end() - 1);

	LOG_DEBUG << "\tParameters : " << result;
	return result;
}
