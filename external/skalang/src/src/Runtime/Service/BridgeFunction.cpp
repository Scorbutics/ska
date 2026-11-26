#include <cassert>
#include <algorithm>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Service/ScopedSymbolTable.h"
#include "BridgeFunction.h"
#include "NodeValue/Symbol.h"

bool ska::BridgeFunction::isVoid() const { return m_function.symbolTable == nullptr || m_function.symbolTable->symbol() == nullptr || m_function.symbolTable->symbol()->nativeType() ==  ExpressionType::VOID; }
bool ska::BridgeFunction::isFactory() const { return m_function.symbolTable != nullptr && m_function.symbolTable->symbol() != nullptr && m_function.symbolTable->symbol()->nativeType() == ExpressionType::OBJECT; }

std::string ska::BridgeField::name() const {
	return symbolTable == nullptr ? "" : symbolTable->name();
}

std::vector<ska::BridgeFunction> ska::BridgeFunction::makeFunctions() const {
    auto result = std::vector<BridgeFunction>{};
    if (m_fields.empty()) {
        return result;
    }
    result.resize(m_fields.size());
    std::transform(m_fields.begin(), m_fields.end(), result.begin(), [](const auto& field) {
        assert(field.symbolTable != nullptr);
        return BridgeFunction{ *field.symbolTable };
    });
    return result;
}

const ska::ScopedSymbolTable& ska::BridgeFunction::symbolTable() const {
    if(m_function.symbolTable == nullptr || m_function.symbolTable->symbol() == nullptr) {
    throw std::runtime_error("bad function symbol");
    }
    return *m_function.symbolTable;
}
