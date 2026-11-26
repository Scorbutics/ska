#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorImport.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"

using GeneratorOperatorCurrent = ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, GeneratorOperatorCurrent);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, GeneratorOperatorCurrent)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>::generate(OperateOn node, GenerationContext& context) {
	auto importGroup = generateNext(context.next(node.GetScriptPathNode(), 1));
	const auto& scriptImportedName = *importGroup.operand().as<StringShared>();
	auto [importedScriptIndex, importedScript] = context.script(scriptImportedName);

	if(importedScript == nullptr /*&& importedScriptIndex == std::numeric_limits<std::size_t>::max()*/) {
		LOG_DEBUG << "%10cUnknown script " << scriptImportedName << ", generating it...";
		auto scriptImported = context.useImport(scriptImportedName);
		assert(scriptImported != nullptr);
		auto scriptContext = context.next(*scriptImported);
		auto instructionsOutput = generateNext(scriptContext);
		scriptContext.generate(std::move(instructionsOutput));
		LOG_DEBUG << "%10cGenerated script " << scriptImportedName;
		std::tie(importedScriptIndex, importedScript) = context.script(scriptImportedName);
	} else {
		LOG_DEBUG << "%10cScript " << scriptImportedName << " was already generated";
	}

	assert(importedScriptIndex != std::numeric_limits<std::size_t>::max());
	importGroup.push(Instruction { Command::SCRIPT, context.queryNextRegister(), Operand { ScriptVariableRef { importedScriptIndex, context.scriptIndex() }, OperandType::BIND_SCRIPT } });
	return importGroup;
}
