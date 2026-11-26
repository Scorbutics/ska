#include "InterpreterCommandScript.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SCRIPT)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& cell = context.currentInstruction().left();
	const auto importedScriptIndex = cell.as<ScriptVariableRef>().variable;
	auto importedScriptExecutionContext = ExecutionContext {context, importedScriptIndex};
	m_interpreter.interpret(importedScriptExecutionContext);
	return importedScriptExecutionContext.generateExportedVariables(importedScriptIndex);
}
