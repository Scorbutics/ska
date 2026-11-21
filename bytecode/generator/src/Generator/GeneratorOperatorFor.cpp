#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFor.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FOR_LOOP>::generate(OperateOn node, GenerationContext& context) {

	auto initGroup = generateNext(context.next(node.GetInitialization()));

	auto conditionGroup = generateNext(context.next(node.GetCondition() ));
	auto bodyGroup = generateNext(context.next(node.GetStatement(), 1));
	auto incrementGroup = generateNext(context.next(node.GetIncrement() ));

	// The loop is left when its condition is false (no condition : an endless loop)
	if (!conditionGroup.operand().empty()) {
		conditionGroup.push(Instruction{ Command::JUMP_NIF, Operand { static_cast<long>(bodyGroup.size() + incrementGroup.size() + 1), OperandType::PURE }, conditionGroup.operand() });
	}
	incrementGroup.push(Instruction{ Command::JUMP_REL, Operand { - static_cast<long>(conditionGroup.size() + bodyGroup.size() + incrementGroup.size() + 1 ), OperandType::PURE } });

	initGroup.push(std::move(conditionGroup));
	initGroup.push(std::move(bodyGroup));
	initGroup.push(std::move(incrementGroup));
	return initGroup;
}
