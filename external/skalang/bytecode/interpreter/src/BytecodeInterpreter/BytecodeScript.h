#pragma once
#include "Generator/Value/BytecodeScriptGeneration.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "Runtime/Service/BridgeFunction.h"

namespace ska {
	namespace bytecode {
		class ScriptExecution;
		class Generator;
		class Interpreter;

		class Script {
		public:
			Script(ScriptCache& cache, const std::string& fullName, std::vector<ska::Token> tokens) :
				m_cache(cache),
				m_serviceGen(cache.emplaceNamed(ScriptGeneration{ m_cache, std::move(tokens), fullName }, true)) {}

			void generate(Generator& generator) {
				m_serviceGen.generate(m_cache, generator);
			}

			std::unique_ptr<Executor> execute(Interpreter& interpreter);

			std::string name() const { return astScript().name(); }

			Script(Script&&) = default;
			Script(const Script&) = delete;
			Script& operator=(const Script&) = delete;
			Script& operator=(Script&&) = delete;

			ScriptAST astScript() const { return m_serviceGen.program(); }

			void fromBridge(BridgeFunction& constructor, ASTNodePtr astRoot, Interpreter& interpreter);

		private:
			Operand findBytecodeMemoryFromSymbol(const ScopedSymbolTable& symbol) const;
			const ScopedSymbolTable& findSymbolFromString(const std::string& key) const;
			const ScopedSymbolTable* findFieldSymbol(const ScopedSymbolTable* constructor, const BridgeField& field) const;

			ScriptCache& m_cache;
			ScriptGeneration& m_serviceGen;
		};
	}
}
