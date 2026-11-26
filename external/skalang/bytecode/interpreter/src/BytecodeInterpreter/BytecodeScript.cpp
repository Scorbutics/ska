#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Service/BridgeFunction.h"
#include "Generator/BytecodeGenerator.h"

#include "Service/ScopedSymbolTable.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Script);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Script)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Script)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::Script)

ska::bytecode::Operand ska::bytecode::Script::findBytecodeMemoryFromSymbol(const ScopedSymbolTable& symbolTable) const {
	assert(symbolTable.symbol() != nullptr);
	auto bytecodeSymbol = m_serviceGen.getSymbol(*symbolTable.symbol());
	if (bytecodeSymbol.has_value()) {
		throw std::runtime_error("unable to find generated symbol \"" + symbolTable.name() + "\" in script \"" + astScript().name() + "\"");
	}
	return bytecodeSymbol.value();
}

const ska::ScopedSymbolTable& ska::bytecode::Script::findSymbolFromString(const std::string& key) const {
	const auto& symbols = astScript().symbols();
	auto* symbolTable = symbols[key];
	if (symbolTable == nullptr) {
		throw std::runtime_error("unable to find AST symbol \"" + key + "\" in root script \"" + astScript().name() + "\"");
	}
	return *symbolTable;
}

const ska::ScopedSymbolTable* ska::bytecode::Script::findFieldSymbol(const ScopedSymbolTable* constructor, const BridgeField& field) const {
	if (field.symbolTable == nullptr) {
		return nullptr;
	}

	if (constructor != nullptr) {
		// field.symbolTable belongs to the template script : the bound one must be retrieved by name from the bound constructor
		return (*constructor)(field.symbolTable->name());
	}

	return &findSymbolFromString(field.symbolTable->name());
}

void ska::bytecode::Script::fromBridge(BridgeFunction& constructor, ASTNodePtr astRoot, Interpreter& interpreter) {
	m_serviceGen.program().fromBridge(std::move(astRoot));

	LOG_DEBUG << "Generating bindings for script " << m_serviceGen.name();

	/* Why don't we use the symbol stored directly in constructor.type().symbol() ? */
	/* Because it still targets the template script type, not the bound script one ! */
	/* (remember, one is - almost - the copy of the other) */
	/* so here we have to iterate through the bound script symbols instead of the template one */
	const ScopedSymbolTable* constructorBoundSymbolTable = !constructor.isVoid() ? &findSymbolFromString(constructor.name()) : nullptr;
	std::size_t bindingId = 0;
	for (const auto& field : constructor.fields()) {
		const ScopedSymbolTable* newerSymbolTable = findFieldSymbol(constructorBoundSymbolTable, field);
		if(newerSymbolTable == nullptr || newerSymbolTable->symbol() == nullptr) {
			auto ss = std::stringstream {};
			assert(field.symbolTable != nullptr && field.symbolTable->symbol() != nullptr);
			ss << "No symbol attached to type " << field.symbolTable->symbol()->type();
			LOG_ERROR << ss.str();
			throw std::runtime_error(ss.str());
		}

		LOG_INFO << "Attaching binding to symbol " << newerSymbolTable->name();
		auto info = m_cache.getSymbolInfoOrNew(m_serviceGen.id(), *newerSymbolTable->symbol());
		auto bindingRef = ScriptVariableRef{ bindingId++, m_serviceGen.id() };
		m_cache.storeBinding(std::make_shared<NativeFunction>(field.callback), bindingRef);
		info.binding = bindingRef.variable;
		info.exported = true;

		//TODO avoid going through symbol info ?
		// Maybe instead move this algorithm directly to generators
		// and here only store the "constructor" variable somewhere in m_serviceGen
		m_cache.setSymbolInfo(*newerSymbolTable->symbol(), std::move(info));
	}

	if (constructorBoundSymbolTable != nullptr && constructorBoundSymbolTable->symbol() != nullptr) {
		auto constructorInfo = m_cache.getSymbolInfoOrNew(m_serviceGen.id(), *constructorBoundSymbolTable->symbol());
		auto bindingRef = ScriptVariableRef{ bindingId++, m_serviceGen.id() };
		m_cache.storeBinding(std::make_shared<NativeFunction>([&constructor](std::vector<NodeValue> params) {
			LOG_INFO << "Parameters of constructor " << constructor.name() << " : ";
			for (const auto& param : params) {
				LOG_INFO << param.convertString();
			}
			constructor.setAdditionalParams(std::move(params));
			return NodeValue{};
		}, true), bindingRef);
		constructorInfo.binding = bindingRef.variable;
		constructorInfo.bindingPassThrough = true;
		m_cache.setSymbolInfo(*constructorBoundSymbolTable->symbol(), std::move(constructorInfo));
	}

	m_serviceGen.generate(m_cache, interpreter.generator());
	LOG_DEBUG << "Generation done for script " << m_serviceGen.name();
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Script::execute(Interpreter& interpreter) {
	return interpreter.interpret(m_serviceGen.id(), m_cache);
}
