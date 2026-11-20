#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorReturn.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::RETURN>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = generateNext(context.next(node.GetValue()));

	LOG_DEBUG << "Returning " << node.GetValue().size() << " fields";

	auto registerV = context.queryNextRegister();
	if(node.GetValue().op() == Operator::USER_DEFINED_OBJECT)  {
		const auto numberOfItemsReturned = static_cast<long>(node.GetValue().size());
		objectResult.push(Instruction { Command::POP_IN_VAR, registerV, Operand { numberOfItemsReturned, OperandType::PURE} });
	} else if (objectResult.size() == 0 && node.GetValue().op() != Operator::SCRIPT_OBJECT) {
		// A script object has no value to move : its fields are exported through their symbol infos
		objectResult.push(Instruction{ Command::MOV, registerV, objectResult.operand() });
	}

	LOG_DEBUG << "Returning : " << objectResult;

	return objectResult;
}
