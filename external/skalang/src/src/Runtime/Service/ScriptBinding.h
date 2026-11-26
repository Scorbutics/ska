#pragma once

#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Runtime/Value/NativeFunction.h"
#include "BridgeASTBuilder.h"
#include "NodeValue/ScriptCacheAST.h"
#include "NodeValue/ScriptASTPtr.h"
#include "NodeValue/ScriptAST.h"
#include "Runtime/Value/NodeValue.h"
#include "Runtime/Value/InterpreterTypes.h"
#include "Runtime/Value/ModuleConfiguration.h"
#include "Runtime/Service/BridgeFunction.h"
#include "Runtime/Value/BuiltinTypeMap.h"

namespace ska {
	class ScriptBindingAST;
	class ScopedSymbolTable;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptBindingAST);

namespace ska {
	class SymbolTable;
	class StatementParser;

	class ScriptBindingAST {
		using ModuleConfiguration = lang::BaseModuleConfiguration;
	public:
		virtual ~ScriptBindingAST() = default;

		void bindFunction(const ScopedSymbolTable& function, decltype(NativeFunction::function) f);

		auto& templateScript() { return *m_templateScript; }

	private:
		void fillConstructorWithBindings(BridgeFunction& constructor);
		void queryAST();

		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(std::function<ReturnType(ParameterTypes...)> f, const std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(BuiltinTypeMap::convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		NativeFunctionPtr makeScriptSideBridge(std::function<ReturnType(ParameterTypes...)> f) {
			auto lambdaWrapped = [f, this](std::vector<NodeValue> v) {
				if constexpr(std::is_same_v<ReturnType, void>) {
					callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>());
					return NodeValue{};
				} else {
					return NodeValue(callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()));
				}
			};

			return std::make_shared<NativeFunction>(static_cast<decltype(NativeFunction::function)> (std::move(lambdaWrapped)));
		}

	protected:
		ScriptBindingAST(
			ModuleConfiguration& config,
			std::string scriptName,
			std::string templateScriptName);

		std::size_t id() const { return m_scriptAst.id(); }
		const auto& name() const { return m_scriptAst.name(); }
		ASTNodePtr buildFunctionsAST(BridgeFunction& constructor);

	private:
		StatementParser& m_parser;
		ScriptASTPtr m_templateScript;

		BridgeASTBuilder m_functionBuilder;
		std::string m_name;
		std::string m_templateName;
		ScriptAST m_scriptAst;
		ScriptCacheAST& m_cacheAst;

		std::unordered_map<std::string, ASTNodePtr> m_imports;
		std::vector<BridgeField> m_bindings;
	};

	template <class Interpreter>
	class ScriptBinding : public ScriptBindingAST {
	using Script = typename InterpreterTypes<Interpreter>::Script;
	using ModuleConfiguration = lang::ModuleConfiguration<Interpreter>;
	public:
		ScriptBinding(
			ModuleConfiguration& moduleConf,
			std::string scriptName,
			std::string templateScriptName) :
			ScriptBindingAST(
				moduleConf,
				scriptName,
				std::move(templateScriptName)),
			m_interpreter(moduleConf.interpreter),
			m_script(moduleConf.scriptCache, ScriptBindingAST::name(), std::vector<Token>{}) {
			if (ScriptBindingAST::id() != m_script.astScript().id()) {
				throw std::runtime_error("script index coherence error while binding a script");
			}
		}

		void buildFunctions(BridgeFunction& constructor) {
			auto astRoot = ScriptBindingAST::buildFunctionsAST(constructor);
			m_script.fromBridge(constructor, std::move(astRoot), m_interpreter);
		}

	private:
		Script m_script;
		Interpreter& m_interpreter;
	};
}
