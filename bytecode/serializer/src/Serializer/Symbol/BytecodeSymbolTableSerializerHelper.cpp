#include "Config/LoggerConfigLang.h"
#include "BytecodeSymbolTableSerializerHelper.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/BytecodeOperandSerializer.h"

#include "Service/ScopedSymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SymbolTableSerializerHelper);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableSerializerHelper)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializerHelper)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::SymbolTableSerializerHelper)

ska::bytecode::SymbolTableSerializerHelper::SymbolTableSerializerHelper(const ScriptCache& cache) :
	m_cache(&cache) {
}

ska::bytecode::TreeSymbolTableMapBuilder& ska::bytecode::SymbolTableSerializerHelper::getMapBuilder(std::size_t id) {
	if (id >= m_mapBuilder.size()) {
		m_mapBuilder.resize(id + 1);
	}

	if (m_mapBuilder[id] == nullptr) {
		m_mapBuilder[id] = std::make_unique<TreeSymbolTableMapBuilder>(m_cache->at(id).program().symbols());
	}

	return *m_mapBuilder[id];
}

std::size_t ska::bytecode::SymbolTableSerializerHelper::scriptOfSymbol(const ScopedSymbolTable& symbolTable) {
	assert(symbolTable.symbol() != nullptr);

	// A symbol table without bytecode info (ex : the returned object table of a factory, named after it)
	// belongs to the same script as its closest ancestor having one. The root symbol table is named after its script.
	const auto* current = &symbolTable;
	while (true) {
		const auto* info = current->symbol() != nullptr ? m_cache->getSymbolInfo(*current->symbol()) : nullptr;
		if (info != nullptr) {
			LOG_DEBUG << "Symbol \"" << symbolTable.name() << "\" is in script \"" << info->script << "\"";
			return info->script;
		}

		const auto& parent = current->parent();
		if (&parent == current) {
			// Root symbol table
			break;
		}
		current = &parent;
	}

	std::size_t scriptId;
	auto scriptIt = m_cache->find(current->name());
	if (scriptIt == m_cache->end()) {
		throw std::runtime_error("unknown ast Symbol \"" + symbolTable.name() + "\" detected during script bytecode serialization");
	}
	scriptId = (*scriptIt)->id();
	LOG_DEBUG << "Symbol \"" << symbolTable.name() << "\" is in script \"" << scriptId << "\"";

	return scriptId;
}

ska::bytecode::Operand ska::bytecode::SymbolTableSerializerHelper::operandOfSymbol(const ScopedSymbolTable& symbolTable) {
	assert(symbolTable.symbol() != nullptr);
	auto scriptId = scriptOfSymbol(symbolTable);
	auto& script = (*m_cache)[scriptId];

	auto operand = script.getSymbol(*symbolTable.symbol());
	if (!operand.has_value()) {
		operand = Operand{ ScriptVariableRef{ scriptId, scriptId }, OperandType::BIND_SCRIPT };
		//throw std::runtime_error("unable to find the matching operand for symbol \"" + symbol.name() + "\" detected during script bytecode serialization");
	}
	LOG_DEBUG << "Symbol \"" << symbolTable.name() << "\" has operand \"" << operand.value() << "\"";
	return operand.value();
}

const std::string& ska::bytecode::SymbolTableSerializerHelper::getScriptName(const std::size_t scriptId) const {
	return m_cache->at(scriptId).name();
}

std::string ska::bytecode::SymbolTableSerializerHelper::getRelativeScriptKey(std::size_t scriptId, const ScopedSymbolTable& value) {
	auto relativeScriptKey = getMapBuilder(scriptId).key(value);
	LOG_DEBUG << "Getting symbol \"" << value.name() << "\" relative script key \"" << relativeScriptKey << "\"";
	return relativeScriptKey;
}

void ska::bytecode::SymbolTableSerializerHelper::writeOperand(SerializerSafeZone<17> safeZone, const Operand& value) {
	OperandSerializer::write(*m_cache, std::move(safeZone), value);
}
