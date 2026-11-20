#pragma once
#include <cassert>
#include <tuple>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Value/BytecodeOperand.h"
#include "Value/BytecodeSymbolInfo.h"
#include "Value/BytecodeScriptGenerationHelper.h"
#include "Value/BytecodeGenerationOutput.h"
#include "Runtime/Value/NativeFunction.h"
#include "Value/BytecodeClosure.h"

namespace ska {
	class ASTNode;
	class ScopedSymbolTable;
	namespace bytecode {
		class GeneratorOperatorUnit;
		class GenerationContext {
		private:
			GenerationContext(GenerationContext& old, const ScriptAST& scriptAst);
			GenerationContext(GenerationContext& old, ScriptGenerationHelper script);
			GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset = 0);
		public:
			GenerationContext(GenerationContext& old);
			GenerationContext(GenerationContext&& old) = default;
			GenerationContext(GenerationOutput& output, ScriptGenerationHelper script);

			GenerationContext next(const ScriptAST& scriptAst);
			GenerationContext next(const ASTNode& node, std::size_t scopeLevelOffset = 0);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }

			auto scope() const { return m_scopeLevel; }

			std::pair<std::size_t, ScriptGeneration*> script(const std::string& fullScriptName);
			std::size_t totalScripts() const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			Register queryNextRegister();
			ScriptASTPtr useImport(const std::string& scriptImported);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;
			const NativeFunction& getBinding(const ScriptVariableRef& bindingRef) const;
			Operand storeBinding(NativeFunctionPtr binding, const ScriptVariableRef& bindingRef);
			void generate(InstructionOutput instructions);
			InstructionOutput querySymbolOrOperand(const ASTNode& node, bool capture = true);
			//OperandUse querySymbol(const Symbol& symbol);
			std::optional<Operand> getSymbol(const Symbol& symbol) const;
			std::size_t exportId(const Symbol& symbol) const;

			const auto scriptIndex() const { return m_script.id(); }
			auto scriptName() const { return m_script.name(); }
			const std::string scriptName(std::size_t index) const;

			InstructionOutput close(const Operand& returnedObject = Operand{});

		private:
			ScriptGenerationHelper& helper();
			const ScriptGenerationHelper& helper() const;
			ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol);
			const ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol) const;

			GenerationOutput& m_generated;
			ScriptGeneration& m_script;
			const ASTNode* const m_pointer {};

			std::unique_ptr<Closure> m_nodeClosure;
			Closure* m_scopeClosure = nullptr;

			std::size_t m_scopeLevel = 0;
		};
	}
}
