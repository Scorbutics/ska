#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorScriptObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/Value/BytecodeSymbolInfo.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = InstructionOutput{ };

	std::size_t childIndex = 0;
	for(auto& field: node) {
		auto valueGroup = generateNext(context.next(*field, 1));

		// Only declarations are fields of the script object.
		// Other top-level statements (expressions, function calls, affectations, if, for, filter...) are only executed,
		// even if they carry a symbol (ex : a function call node refers to the called function symbol).
		// Keep this in sync with TypeBuilderOperator<SCRIPT_OBJECT>.
		if (field->op() != Operator::DECLARATION || field->symbol() == nullptr) {
			objectResult.push(std::move(valueGroup));
			continue;
		}

		auto* oldSymbolInfo = context.getSymbolInfo(*field);
		auto symbolInfo = SymbolInfo { context.scope() + 1, field->name(), context.scriptIndex() };
		if (oldSymbolInfo != nullptr) {
			symbolInfo.binding = oldSymbolInfo->binding;
			symbolInfo.bindingPassThrough = oldSymbolInfo->bindingPassThrough;
		}
		symbolInfo.childIndex = childIndex;
		symbolInfo.exported = true;
		LOG_INFO << "%12cRegistering symbol info " << symbolInfo << " for field node " << *field << " (this symbol is exported)";

		// No value is pushed on the stack for script fields : unlike a user defined object (built with PUSH + POP_IN_VAR),
		// a script exports its fields through their symbol info (see ScriptCache::getExportedSymbols).
		context.setSymbolInfo(*field, std::move(symbolInfo));

		objectResult.push(std::move(valueGroup));
		childIndex++;
	}

	LOG_INFO << "Creating user defined object ";
	LOG_DEBUG << objectResult;

	return objectResult;
}
