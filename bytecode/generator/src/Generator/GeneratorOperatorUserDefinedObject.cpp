#include "Config/LoggerConfigLang.h"
#include <string>
#include <vector>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/Value/BytecodeSymbolInfo.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = InstructionOutput{ };

	// Fields are registered before generating their values : a method can access a field of another instance of the same
	// object type (ex : a recursive structure), including itself, while it is generated.
	std::size_t childIndex = 0;
	for(auto& field: node) {
		auto* oldSymbolInfo = context.getSymbolInfo(*field);
		auto symbolInfo = SymbolInfo { context.scope() + 1, field->name(), context.scriptIndex() };
		if (oldSymbolInfo != nullptr) {
			symbolInfo.binding = oldSymbolInfo->binding;
			symbolInfo.bindingPassThrough = oldSymbolInfo->bindingPassThrough;
		}
		symbolInfo.childIndex = childIndex;
		LOG_INFO << "%12cRegistering symbol info " << symbolInfo << " for field node " << *field << " (this symbol is not exported)";

		context.setSymbolInfo(*field, std::move(symbolInfo));
		childIndex++;
	}

	auto fieldGroups = std::vector<InstructionOutput>{};
	for(auto& field: node) {
		fieldGroups.push_back(generateNext(context.next(*field, 1)));
	}

	// Fields are pushed once all of them are generated : a field captured by a method of the object (ex : read in a getter)
	// is only known as captured after the method generation. Such a field is pushed as its cell,
	// so that the object field and the variable captured by the method share the same storage.
	const auto* scope = context.functionScope();
	for (auto& valueGroup : fieldGroups) {
		const auto fieldOperand = valueGroup.operand();
		const auto pushCommand = scope != nullptr && scope->isCaptured(fieldOperand) ? Command::PUSH_CELL : Command::PUSH;
		valueGroup.push(Instruction { pushCommand, fieldOperand });
		objectResult.push(std::move(valueGroup));
	}

	LOG_INFO << "Creating user defined object ";
	LOG_DEBUG << objectResult;

	return objectResult;
}
