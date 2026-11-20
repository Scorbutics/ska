#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "GeneratorOperatorFieldAccess.h"
#include "GeneratorOperatorArray.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>)

namespace ska {
	namespace bytecode {
		template <class Generator>
		static InstructionOutput GenerateStore(Generator& generator, const ASTNode& dest, const Operand& value, GenerationContext& context) {
			// Field of a custom object : "object.field = value"
			if (dest.op() == Operator::FIELD_ACCESS) {
				auto fieldAccess = OperationType<Operator::FIELD_ACCESS>{ dest };
				const auto index = FieldAccessIndex(fieldAccess, context);
				auto result = generator.generateNext(context.next(fieldAccess.GetObjectNameNode()));
				result.push(Instruction { Command::ARR_SET, result.operand(), index, value });
				return result;
			}

			// Array cell : "array[index] = value"
			if (dest.op() == Operator::ARRAY_USE) {
				auto arrayUse = OperationType<Operator::ARRAY_USE>{ dest };
				auto result = generator.generateNext(context.next(arrayUse.GetArray()));
				auto indexComputing = generator.generateNext(context.next(arrayUse.GetArrayIndex()));
				const auto array = result.operand();
				const auto index = indexComputing.operand();
				result.push(std::move(indexComputing));
				result.push(Instruction { Command::ARR_SET, array, index, value });
				return result;
			}

			// Variable
			auto variable = dest.isSymbolicLeaf() ? context.querySymbolOrOperand(dest) : generator.generateNext(context.next(dest));
			auto variableDestination = variable.operand();
			variable.push(Instruction { Command::MOV, variableDestination, value });
			return variable;
		}

		template <class Generator>
		static InstructionOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto finalGroup = generator.generateNext(context.next(node));
			auto operandDestination = finalGroup.operand();
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !finalGroup.empty()) {
				LOG_DEBUG << "Creating store instruction with operand group " << finalGroup;
				finalGroup.push(GenerateStore(generator, dest, operandDestination, context));
				LOG_DEBUG << "\tin operand " << operandDestination;
			}
			return finalGroup;
		}
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::PARAMETER_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return { context.querySymbolOrOperand(node.GetVariableNameNode()) };
}
