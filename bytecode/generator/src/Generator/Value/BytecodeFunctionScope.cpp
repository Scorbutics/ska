#include "BytecodeFunctionScope.h"
#include "NodeValue/AST.h"
#include "Service/ScopedSymbolTable.h"
#include "Generator/BytecodeCommand.h"

ska::bytecode::FunctionScope::FunctionScope(const ASTNode& functionNode, FunctionScope* parent, std::size_t scriptIndex) :
	m_node(&functionNode),
	m_parent(parent),
	m_script(scriptIndex) {
	assert(m_node->symbolTable() != nullptr && "a function node must have a symbol table");
}

const ska::ScopedSymbolTable* ska::bytecode::FunctionScope::symbolTable() const {
	return m_node->symbolTable();
}

const ska::bytecode::FunctionScope* ska::bytecode::FunctionScope::declaringScope(const ScopedSymbolTable& variableTable) const {
	// The scope where the variable is declared : its parent symbol table (a function, a block, an object literal...)
	const auto* table = &variableTable.parent();
	while (true) {
		for (const auto* scope = this; scope != nullptr; scope = scope->m_parent) {
			if (scope->symbolTable() == table) {
				return scope;
			}
		}

		const auto* parentTable = &table->parent();
		if (parentTable == table) {
			// Root symbol table reached : script level variable
			return nullptr;
		}
		table = parentTable;
	}
}

ska::bytecode::OperandUse ska::bytecode::FunctionScope::local(const Symbol& symbol, Cursor position) {
	auto it = m_locals.find(&symbol);
	const auto isNew = it == m_locals.end();
	if (isNew) {
		it = m_locals.emplace(&symbol, m_locals.size()).first;
	}
	auto result = OperandUse{ ScriptVariableRef{ it->second, m_script }, OperandType::LOCAL, std::move(position) };
	result.isFirstTimeUsed = isNew;
	return result;
}

ska::bytecode::Operand ska::bytecode::FunctionScope::capturedLocal(const Symbol& symbol) {
	auto result = local(symbol);
	m_capturedLocals.insert(result.as<ScriptVariableRef>().variable);
	return result;
}

ska::bytecode::OperandUse ska::bytecode::FunctionScope::variable(const Symbol& symbol, const FunctionScope& declaring, Cursor position) {
	if (&declaring == this) {
		return local(symbol, std::move(position));
	}

	assert(m_parent != nullptr && "a variable of an enclosing function requires an enclosing function");

	auto it = m_upvalueIndexes.find(&symbol);
	if (it == m_upvalueIndexes.end()) {
		// The enclosing function provides the cell : either its own captured local variable, or one of its upvalues
		auto source = m_parent == &declaring ? m_parent->capturedLocal(symbol) : Operand{ m_parent->variable(symbol, declaring) };
		it = m_upvalueIndexes.emplace(&symbol, m_upvalueSources.size()).first;
		m_upvalueSources.push_back(std::move(source));
	}

	return OperandUse{ ScriptVariableRef{ it->second, m_script }, OperandType::UPVAL, std::move(position) };
}

ska::bytecode::Register ska::bytecode::FunctionScope::nextRegister() {
	auto result = Register{ ScriptVariableRef { m_register++, m_script }, OperandType::REG };
	result.isFirstTimeUsed = true;
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::FunctionScope::prologue() const {
	auto result = InstructionOutput{};
	for (const auto localIndex : m_capturedLocals) {
		result.push(Instruction{ Command::CELL, Operand{ ScriptVariableRef{ localIndex, m_script }, OperandType::LOCAL } });
	}
	return result;
}

bool ska::bytecode::FunctionScope::isCaptured(const Operand& operand) const {
	return operand.type() == OperandType::LOCAL && m_capturedLocals.find(operand.as<ScriptVariableRef>().variable) != m_capturedLocals.end();
}
