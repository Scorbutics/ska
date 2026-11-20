#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFilter.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FILTER>::generate(OperateOn node, GenerationContext& context) {
	auto initGroup = generateNext(context.next(node.GetCollection()));

	auto collectionContainer = initGroup.operand();
	auto collectionLengthVariable = context.queryNextRegister();
	initGroup.push(Instruction{ Command::ARR_LENGTH, collectionLengthVariable, collectionContainer });

	auto iteratorRegister = node.GetCollectionIteratorIndex().logicalEmpty() ? context.queryNextRegister() : context.querySymbolOrOperand(node.GetCollectionIteratorIndex());
	auto iteratorRegisterOperand = iteratorRegister.operand();
	initGroup.push(std::move(iteratorRegister));
	initGroup.push(Instruction{ Command::MOV, iteratorRegisterOperand, OperandUse{ 0l, OperandType::PURE} });

	auto conditionGroup = InstructionOutput{ Instruction{ Command::SUB_I, context.queryNextRegister(), iteratorRegisterOperand, collectionLengthVariable} };
	conditionGroup.push(Instruction { Command::TEST_L, conditionGroup.operand(), conditionGroup.operand() });

	auto collectionElement = context.querySymbolOrOperand(node.GetCollectionIterator());
	auto collectionElementOperand = collectionElement.operand();
	auto bodyGroup = std::move(collectionElement);
	bodyGroup.push(Instruction{ Command::ARR_ACCESS, collectionElementOperand, collectionContainer, iteratorRegisterOperand});
	bodyGroup.push(generateNext(context.next(node.GetStatement(), 1)));

	auto incrementGroup = InstructionOutput{ Instruction { Command::ADD_I, iteratorRegisterOperand, iteratorRegisterOperand, Operand{ 1l, OperandType::PURE } } };

	// Post adding relatives jump, now we now the generated instruction pack size
	conditionGroup.push(Instruction{ Command::JUMP_NIF, Operand { static_cast<long>(bodyGroup.size() + incrementGroup.size() + 1), OperandType::PURE }, conditionGroup.operand() });
	incrementGroup.push(Instruction{ Command::JUMP_REL, Operand { -static_cast<long>(conditionGroup.size() + bodyGroup.size() + incrementGroup.size() + 1), OperandType::PURE } });

	initGroup.push(std::move(conditionGroup));
	initGroup.push(std::move(bodyGroup));
	initGroup.push(std::move(incrementGroup));
	return initGroup;
}
