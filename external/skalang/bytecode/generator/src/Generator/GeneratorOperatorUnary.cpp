#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::UNARY>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::UNARY>)

namespace ska {
	namespace bytecode {
		static InstructionOutput CommonGenerate(GenerationContext& context) {
			const auto& node = context.pointer();
			auto result = context.querySymbolOrOperand(node);
			LOG_DEBUG << "Generating unary : " << result;
			return { std::move(result) };
		}
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}
