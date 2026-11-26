#pragma once
#include <string>
#include <vector>
#include "Runtime/Value/NativeFunction.h"
#include "Runtime/Value/NodeValue.h"

namespace ska {
  class ScopedSymbolTable;
  struct BridgeField {
    using Callback = typename NativeFunction::Callback;

    BridgeField() = default;
    ~BridgeField() = default;

    BridgeField(const ScopedSymbolTable& fullType) : symbolTable(&fullType) {}

    std::string name() const;

  public:
    const ScopedSymbolTable* symbolTable = nullptr;
    Callback callback;
  };

  class BridgeFunction {
  public:
    BridgeFunction() = default;
    BridgeFunction(const ScopedSymbolTable& fullType) : m_function(fullType) {}
    BridgeFunction(BridgeField function) : m_function(std::move(function)) {}

    void bindField(BridgeField field) {
      m_fields.push_back(std::move(field));
    }

    bool hasFields() const { return !m_fields.empty(); }

    std::vector<BridgeFunction> makeFunctions() const;

    std::string name() const { return m_function.name(); }

    const ScopedSymbolTable& symbolTable() const;

    bool isVoid() const;
    bool isFactory() const;

    const BridgeField::Callback& callback() const { return m_function.callback; }
    const std::vector<BridgeField>& fields() const { return m_fields; }

    void setAdditionalParams(std::vector<NodeValue> params) { m_additionalParamValues = std::move(params); }
    template <class T>
    T& param(std::size_t index) {
        return m_additionalParamValues[index].nodeval<T>();
    }

  private:
    BridgeField m_function;
    std::vector<NodeValue> m_additionalParamValues;
    std::vector<BridgeField> m_fields;
  };

}
