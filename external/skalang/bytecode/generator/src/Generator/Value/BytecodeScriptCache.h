#pragma once
#include "BytecodeScriptGeneration.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Runtime/Value/NativeFunction.h"
#include "BytecodeExport.h"

namespace ska {
  class Symbol;
	class ASTNode;
  namespace bytecode {
    class ScriptCache :
      public order_indexed_string_map<ScriptGeneration> {
				using Parent = order_indexed_string_map<ScriptGeneration>;
    public:
        using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;

	    bool isGenerated(std::size_t index) const;

	    void setSymbolInfo(const ASTNode& node, SymbolInfo info);
	    void setSymbolInfo(const Symbol& symbol, SymbolInfo info);
	    const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
	    const SymbolInfo* getSymbolInfo(const ASTNode& node) const;
        SymbolInfo getSymbolInfoOrNew(std::size_t scriptIndex, const Symbol& symbol) const;

	    const ExportSymbolContainer& getExportedSymbols(std::size_t scriptIndex);

        void storeBinding(NativeFunctionPtr binding, const ScriptVariableRef& bindingRef);
        const NativeFunction& getBinding(const ScriptVariableRef& bindingRef) const;

        ScriptCacheAST astCache;

        void printDebugInfo(std::ostream& stream) const;

    private:
      SymbolInfosContainer m_symbolInfo;
      order_indexed_string_map<NativeFunctionPtr> m_bindings;
    };
  }
}
