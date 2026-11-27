#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "GeneratorOperatorConverterCall.h"
#include "GeneratorOperatorLanguageValues.h"
#include "ComputingOperations/BytecodeTypeConversion.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::CONVERTER_CALL>::generate(OperateOn node, GenerationContext& context) {
	const auto& originalType = node.GetOriginalType();
	const auto& typeToConvertIn = node.GetCalledConverterType();

	auto objectNodeGeneration = generateNext(context.next(node.GetObjectNode()));

	// A value of an enum is its name : converted to a string, it stays the same ; a string converted to a value of an enum
	// is checked against the names of its values
	if (originalType == ExpressionType::ENUM && typeToConvertIn == ExpressionType::STRING) {
		return objectNodeGeneration;
	}
	if (originalType == ExpressionType::STRING && typeToConvertIn == ExpressionType::ENUM) {
		auto names = GenerateStringArray(EnumValueNames(*typeToConvertIn.symbolTable()), context);
		const auto namesOperand = names.operand();
		const auto text = objectNodeGeneration.operand();
		objectNodeGeneration.push(std::move(names));
		objectNodeGeneration.push(Instruction{ Command::ENUM_CHECK, context.queryNextRegister(), text, namesOperand });
		return objectNodeGeneration;
	}
	if (originalType == ExpressionType::ENUM && typeToConvertIn == ExpressionType::ENUM) {
		return objectNodeGeneration;
	}

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
