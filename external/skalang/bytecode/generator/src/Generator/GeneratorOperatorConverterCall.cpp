#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "GeneratorOperatorConverterCall.h"
#include "ComputingOperations/BytecodeTypeConversion.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>::generate(OperateOn node, GenerationContext& context) {
	const auto& originalType = node.GetOriginalType();
	const auto& typeToConvertIn = node.GetCalledConverterType();

	auto objectNodeGeneration = generateNext(context.next(node.GetObjectNode()));

	if (originalType != typeToConvertIn) {
		auto conversionResult = TypeConversion(LogicalOperator::EQUAL, originalType, typeToConvertIn);

		if (conversionResult.container.empty()) {
			auto ss = std::stringstream{};
			ss << "unable to convert type \"" << originalType << "\" to \"" << typeToConvertIn << "\"";
			throw std::runtime_error(ss.str());
		}
		objectNodeGeneration.push(Instruction{ conversionResult.container[0], context.queryNextRegister(), objectNodeGeneration.operand() });

	}

	return objectNodeGeneration;
}
