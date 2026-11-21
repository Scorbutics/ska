#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_TYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	auto totalPushedElements = applyGenerator(ApplyNOperations<Command::PUSH, decltype(node.GetArrayContent().begin())>, result, context, node.GetArrayContent().begin(), node.GetArrayContent().end());
	result.push(Instruction { Command::POP_IN_ARR, context.queryNextRegister(), Operand { static_cast<long>(totalPushedElements), OperandType::PURE} });
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };

	auto arrayAccessedComputing = generateNext(context.next(node.GetArray()));
	auto arrayIndexComputing = generateNext(context.next(node.GetArrayIndex()));

	auto arrayIndex = arrayIndexComputing.operand();
	auto arrayAccessed = arrayAccessedComputing.operand();
	result.push(std::move(arrayIndexComputing));
	result.push(std::move(arrayAccessedComputing));
	const auto containerType = node.GetArray().type();
	const auto command = containerType == ExpressionType::STRING ? Command::STR_ACCESS : (containerType == ExpressionType::MAP ? Command::MAP_ACCESS : Command::ARR_ACCESS);
	result.push(Instruction{ command, context.queryNextRegister(), std::move(arrayAccessed), std::move(arrayIndex) });

	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::MAP_DECLARATION>::generate(OperateOn, GenerationContext& context) {
	auto result = InstructionOutput{ };
	result.push(Instruction { Command::MAP_NEW, context.queryNextRegister() });
	return result;
}
