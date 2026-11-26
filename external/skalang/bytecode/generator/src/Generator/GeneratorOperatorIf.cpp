#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorIf.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::IF>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext(context.next(node.GetCondition()));
	auto ifGroup = generateNext(context.next(node.GetIfStatement(), 1));
	conditionGroup.push(Instruction{ Command::JUMP_NIF, Operand { static_cast<long>(ifGroup.size()), OperandType::PURE}, conditionGroup.operand() });
	conditionGroup.push(std::move(ifGroup));
	return conditionGroup;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::IF_ELSE>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext(context.next(node.GetCondition()));
	auto ifGroup = generateNext(context.next(node.GetIfStatement(), 1));
	auto elseGroup = generateNext(context.next(node.GetElseStatement(), 1));

	ifGroup.push(Instruction{ Command::JUMP_REL, Operand { static_cast<long>(elseGroup.size()), OperandType::PURE } });

	conditionGroup.push(Instruction{ Command::JUMP_NIF, Operand { static_cast<long>(ifGroup.size()), OperandType::PURE }, conditionGroup.operand()  });
	conditionGroup.push(std::move(ifGroup));
	conditionGroup.push(std::move(elseGroup));
	return conditionGroup;
}
