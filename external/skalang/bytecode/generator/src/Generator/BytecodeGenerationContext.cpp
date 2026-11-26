#include "Runtime/Value/PlainMemoryTable.h"
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output, ScriptGenerationHelper script) :
	m_generated(output),
	m_script(m_generated.emplaceNamed( ScriptGeneration{ std::move(script) } )),
	m_pointer(&m_script.rootASTNode()),
	m_ownedFunctionScope(m_pointer->op() == Operator::FUNCTION_DECLARATION ? std::make_unique<FunctionScope>(*m_pointer, nullptr, m_script.id()) : nullptr),
	m_functionScope(m_ownedFunctionScope.get()) {
}

ska::bytecode::GenerationContext ska::bytecode::GenerationContext::next(const ASTNode &node, std::size_t scopeLevelOffset) {
    return { *this, node, scopeLevelOffset };
}

ska::bytecode::GenerationContext ska::bytecode::GenerationContext::next(const ScriptAST& scriptAst) {
    return { *this, scriptAst };
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ScriptAST& scriptAst) :
	m_generated(old.m_generated),
	m_script(m_generated.emplaceNamed(ScriptGeneration { ScriptGenerationHelper{m_generated, scriptAst}})),
	m_pointer(&m_script.rootASTNode()),
	m_ownedFunctionScope(m_pointer->op() == Operator::FUNCTION_DECLARATION ? std::make_unique<FunctionScope>(*m_pointer, nullptr, m_script.id()) : nullptr),
	m_functionScope(m_ownedFunctionScope.get()) {
}


void ska::bytecode::GenerationContext::generate(InstructionOutput instructions) {
	m_script.generate(m_generated, std::move(instructions));
}

std::size_t ska::bytecode::GenerationContext::totalScripts() const {
	return m_generated.size();
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, ScriptGenerationHelper script) :
	m_generated(old.m_generated),
	m_script(m_generated.emplaceNamed(ScriptGeneration { std::move(script) })),
	m_pointer(&m_script.rootASTNode()),
	m_ownedFunctionScope(m_pointer->op() == Operator::FUNCTION_DECLARATION ? std::make_unique<FunctionScope>(*m_pointer, nullptr, m_script.id()) : nullptr),
	m_functionScope(m_ownedFunctionScope.get()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_pointer(&m_script.rootASTNode()),
	m_ownedFunctionScope(m_pointer->op() == Operator::FUNCTION_DECLARATION ? std::make_unique<FunctionScope>(*m_pointer, old.m_functionScope, m_script.id()) : nullptr),
	m_functionScope(m_ownedFunctionScope != nullptr ? m_ownedFunctionScope.get() : old.m_functionScope) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_pointer(&node),
	m_scopeLevel(old.m_scopeLevel + scopeLevelOffset),
	m_ownedFunctionScope(m_pointer->op() == Operator::FUNCTION_DECLARATION ? std::make_unique<FunctionScope>(*m_pointer, old.m_functionScope, m_script.id()) : nullptr),
	m_functionScope(m_ownedFunctionScope != nullptr ? m_ownedFunctionScope.get() : old.m_functionScope) {
}

ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::helper() {
	return m_script.helper();
}

const ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::helper() const {
	return m_script.helper();
}

ska::bytecode::Register ska::bytecode::GenerationContext::queryNextRegister() {
	// Registers are temporaries of the current function call frame (or of the script, at script level)
	return m_functionScope != nullptr ? m_functionScope->nextRegister() : m_script.helper().queryNextRegister();
}

std::pair<std::size_t, ska::bytecode::ScriptGeneration*> ska::bytecode::GenerationContext::script(const std::string& fullScriptName) {
	return std::make_pair(m_generated.id(fullScriptName), m_generated.atOrNull(fullScriptName));
}

void ska::bytecode::GenerationContext::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	m_generated.setSymbolInfo(node, std::move(info));
}
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const Symbol& symbol) const { return m_generated.getSymbolInfo(symbol); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const ASTNode& node) const { return m_generated.getSymbolInfo(node); }

std::size_t ska::bytecode::GenerationContext::exportId(const Symbol& symbol) const {
	auto& script = scriptOfSymbol(symbol);
	auto& exports = m_generated.getExportedSymbols(script.id());

	if (exports.atOrNull(&symbol) == nullptr) {
		throw std::runtime_error("unable to find symbol \"" + symbol.name() + "\" in exports of script \"" + script.name() + "\"");
	}

	return exports.id(&symbol);
}

ska::bytecode::InstructionOutput ska::bytecode::GenerationContext::querySymbolOrOperand(const ASTNode& node) {
	return querySymbolOrOperand(node, m_functionScope);
}

ska::bytecode::InstructionOutput ska::bytecode::GenerationContext::querySymbolOrOperandFromEnclosingFunction(const ASTNode& node) {
	return querySymbolOrOperand(node, m_functionScope == nullptr ? nullptr : m_functionScope->parent());
}

ska::bytecode::InstructionOutput ska::bytecode::GenerationContext::querySymbolOrOperand(const ASTNode& node, FunctionScope* scope) {
	if (node.symbol() == nullptr) {
		// Literal
		return { helper().querySymbolOrOperand(node) };
	}

	// A variable declared in a function (the current one or an enclosing one) lives in a call frame
	const auto* symbolInfo = m_generated.getSymbolInfo(*node.symbol());
	const auto isFromAnotherScript = symbolInfo != nullptr && symbolInfo->script != m_script.id();
	if (scope != nullptr && !isFromAnotherScript && node.symbolTable() != nullptr) {
		const auto* declaringScope = scope->declaringScope(*node.symbolTable());
		if (declaringScope != nullptr) {
			return { scope->variable(*node.symbol(), *declaringScope, node.positionInScript()) };
		}
	}

	// Script level variable
	return { scriptOfSymbol(*node.symbol()).querySymbolOrOperand(node) };
}

/*
ska::bytecode::OperandUse ska::bytecode::GenerationContext::querySymbol(const Symbol& symbol) {
	return scriptOfSymbol(symbol).querySymbol(symbol);
}*/

std::optional<ska::bytecode::Operand> ska::bytecode::GenerationContext::getSymbol(const Symbol& symbol) const {
	return scriptOfSymbol(symbol).getSymbol(symbol);
}

ska::bytecode::Operand ska::bytecode::GenerationContext::storeBinding(NativeFunctionPtr binding, const ScriptVariableRef& bindingRef) {
	m_generated.storeBinding(std::move(binding), bindingRef);
	return ska::bytecode::Operand{ bindingRef, OperandType::BIND_NATIVE };
}

const ska::NativeFunction& ska::bytecode::GenerationContext::getBinding(const ScriptVariableRef& bindingRef) const {
	return m_generated.getBinding(bindingRef);
}

ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		auto basicInfos = SymbolInfo{m_script.id()};
		m_generated.setSymbolInfo(symbol, std::move(basicInfos));
		return helper();
	}
	return m_generated.at(scriptIt->script).helper();
}

const ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) const {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		return helper();
	}
	return m_generated.at(scriptIt->script).helper();
}

ska::ScriptASTPtr ska::bytecode::GenerationContext::useImport(const std::string& scriptImported) {
	return helper().useImport(scriptImported);
}

const std::string ska::bytecode::GenerationContext::scriptName(std::size_t index) const {
	if(m_generated.size() > index) {
		return "";
	}
	return m_generated.at(index).name();
}
