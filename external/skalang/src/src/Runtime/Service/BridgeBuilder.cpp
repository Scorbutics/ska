#include "Config/LoggerConfigLang.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "BridgeBuilder.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/Symbol.h"
#include "Service/ASTFactory.h"
#include "Service/ScopedSymbolTable.h"

ska::BridgeBuilderASTTemplateLooker::BridgeBuilderASTTemplateLooker(const ScriptAST& templateParent, std::string constructorName) :
  m_parent(templateParent.handle()),
  m_name(std::move(constructorName)),
  m_templateTable(m_name.empty() ? &m_parent->symbols().current() : m_parent->symbols().lookup(ska::SymbolTableLookup::direct(m_name), ska::SymbolTableNested::current())) {
}

const ska::ScopedSymbolTable& ska::BridgeBuilderASTTemplateLooker::variable(const std::string& name) const {
  const auto* symbolTable = m_parent->symbols()[name];
  if(symbolTable == nullptr || symbolTable->symbol() == nullptr) {
    throw std::runtime_error("unable to find variable \"" + name + "\" in template script \"" + m_parent->name() + "\"");
  }
  return *symbolTable;
}

const ska::ScopedSymbolTable* ska::BridgeBuilderASTTemplateLooker::constructor() const {
    return m_name.empty() ? nullptr : &variable(m_name);
}

const ska::ScopedSymbolTable& ska::BridgeBuilderASTTemplateLooker::field(const std::string& name) const {
    const ScopedSymbolTable* symbolTable;
    if (m_templateTable == nullptr) {
        symbolTable = m_parent->symbols()[name];
    } else {
        symbolTable = (*m_templateTable)(name);
    }

    if (symbolTable == nullptr || symbolTable->symbol() == nullptr) {
        throw std::runtime_error("unable to find field \"" + name + (m_name.empty() ? "" : "\" in constructor \"" + m_name) + "\" of template script \"" + m_parent->name() + "\"");
    }
    return *symbolTable;
}

bool ska::BridgeBuilderASTTemplateLooker::hasField(const std::string& name) const {
  return (*m_templateTable)(name) != nullptr;
}
