#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>) << "%01c"

namespace ska {
	namespace bytecode {
		static InstructionOutput AddRelativeJumpInstruction(GenerationContext& context, InstructionOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Operand { static_cast<long>(output.size()), OperandType::PURE }};
			auto result = InstructionOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			const auto retOperand = result.operand();
			result.push(retOperand);
			return result;
		}
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype of \"" << node.GetFunctionName() << "\"...";
	auto valueGroup = generateNext(context.next(node.GetFunctionPrototype()));

	LOG_DEBUG << "Generating body...";
	const auto* functionSymbolInfo = context.getSymbolInfo(node.GetFunction());
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() ? "none" : "with binding");
	if (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() || functionSymbolInfo->bindingPassThrough) {
		valueGroup.push(generateNext(context.next(node.GetFunctionBody(), 1)));
	}

	LOG_DEBUG << "\nGenerated " << valueGroup << " with value " << valueGroup.operand();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().back();
	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	const auto returnValueOperand = valueGroup.operand();
	valueGroup.push(Instruction{ Command::RET, isVoidReturningFunction ? Operand{} : returnValueOperand });

	// Locals and registers live in the call frame of each call : no clean-up required.
	// The prologue makes a cell of each local variable captured by a nested function (known once the body is generated).
	auto* scope = context.functionScope();
	assert(scope != nullptr);
	auto body = scope->prologue();
	body.push(std::move(valueGroup));
	auto fullFunction = AddRelativeJumpInstruction(context, std::move(body));

	// Closure creation, executed in the enclosing function frame when the declaration is reached :
	// the cells captured by the function are pushed, then END builds the function value with them.
	const auto& upvalueSources = scope->upvalueSources();
	for (const auto& source : upvalueSources) {
		fullFunction.push(Instruction{ Command::PUSH_CELL, source });
	}

	auto symbolFunction = context.querySymbolOrOperandFromEnclosingFunction(node.GetFunction());
	auto functionOperand = symbolFunction.operand();
	fullFunction.push(std::move(symbolFunction));
	fullFunction.push(Instruction{
		Command::END,
		functionOperand,
		Operand { -static_cast<long>(fullFunction.size()), OperandType::PURE },
		upvalueSources.empty() ? Operand{} : Operand { static_cast<long>(upvalueSources.size()), OperandType::PURE } });

	return fullFunction;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	// Obvisouly we pop in reverse order, because that's how a stack works (we previously pushed parameters in order in generation of Operator::FUNCTION_CALL)
	// Also, we avoid index 0, which is return type in reverse order
	applyGenerator(ApplyNOperations<Command::POP, decltype(node.begin())>, result, context, node.begin(), node.end() - 1);
	LOG_DEBUG << "\tParameters : " << result;
	return result;
}

static bool IsInstructionCommandClosed(const ska::bytecode::InstructionOutput& pack) {
	if (pack.back() == nullptr) {
		return false;
	}

	switch (pack.back()->command()) {
	case ska::bytecode::Command::ARR_LENGTH:
		return true;
	default:
		return false;
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto preCallValue = generateNext(context.next(node.GetFunctionNameNode()));
	LOG_DEBUG << "Function call : "<< node.GetFunctionNameNode().name() << " of type " << node.GetFunctionType();

	// Handle built-in type functions traduced directly to a bytecode instruction
	const auto* functionTypeSymbol = node.GetFunctionNameNode().symbol();
	if (IsInstructionCommandClosed(preCallValue)) {
		return preCallValue;
	}

	// Handle custom type functions
	const auto* functionSymbolInfo = functionTypeSymbol == nullptr ? nullptr : context.getSymbolInfo(*functionTypeSymbol);
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() ? "none" : "with binding");

	auto callInstruction = InstructionOutput {};
	if (functionSymbolInfo != nullptr && functionSymbolInfo->binding != std::numeric_limits<std::size_t>::max()) {
		callInstruction.push(Instruction{ Command::BIND, Operand { ScriptVariableRef{ functionSymbolInfo->binding, functionSymbolInfo->script}, OperandType::BIND_NATIVE}, Operand {static_cast<long>(node.GetFunctionParameterSize()), OperandType::PURE} });
		if (functionSymbolInfo->bindingPassThrough) {
			callInstruction.push(Instruction{ Command::JUMP_ABS, std::move(preCallValue.operand()) });
		}
	} else {
		callInstruction.push(Instruction { Command::JUMP_ABS, std::move(preCallValue.operand()) });
	}
	auto result = std::move(preCallValue);

	applyGenerator(ApplyNOperations<Command::PUSH, decltype(node.rbegin())>, result, context, node.rbegin(), node.rend());
	LOG_DEBUG << " PUSH result : " << result;

	result.push(std::move(callInstruction));
	if (node.GetFunctionType().back() != ExpressionType::VOID) {
		result.push(Instruction{ Command::POP, context.queryNextRegister()});
	}

	LOG_DEBUG << "Output : " << result;
	return result;
}
