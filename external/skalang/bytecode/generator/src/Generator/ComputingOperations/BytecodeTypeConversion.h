#pragma once
#include <vector>
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeInstructionOutput.h"
#include "NodeValue/LogicalOperator.h"
#include "NodeValue/ExpressionType.h"

namespace ska {
	struct Type;
	namespace bytecode {
		class Script;
		struct TypedOperandRef {
			TypedOperandRef(const Type& type, const Operand operand) : type(type), operand(operand) {}
			const Type& type;
			const Operand operand;
		};

		InstructionOutput TypeConversionBinary(LogicalOperator logicalOperator, const TypedOperandRef& node1, const TypedOperandRef& node2, const TypedOperandRef& destination);
		
		enum class OperationType {
			SPLIT,
			FULL_FIRST
		};

		template <class T>
		struct OperationList {
			OperationType type = OperationType::FULL_FIRST;
			std::vector<T> container;
		};

		using CommandList = OperationList<Command>;

		CommandList TypeConversion(LogicalOperator logicalOperator, const Type& operandType, const Type& operand2Type, bool reverseOrder = false);
	}
}
