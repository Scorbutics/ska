#include <fstream>
#include <unordered_set>
#include "Config/LoggerConfigLang.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "ScriptBinding.h"

#include "Service/ScopedSymbolTable.h"
#include "Service/ASTFactory.h"
#include "Service/ScriptNameBuilder.h"
#include "Service/StatementParser.h"

ska::ScriptBindingAST::ScriptBindingAST(
	ModuleConfiguration& config,
	std::string scriptName,
	std::string templateName) :
	m_parser(config.parser),
	m_functionBuilder(config.typeBuilder, config.reservedKeywords),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_templateName(ScriptNameDeduce("", templateName)),
	m_scriptAst(config.scriptAstCache, m_name, std::vector<Token>{}),
	m_cacheAst(config.scriptAstCache) {
	queryAST();
}

void ska::ScriptBindingAST::bindFunction(const ScopedSymbolTable& function, decltype(NativeFunction::function) f) {
	SLOG(LogLevel::Debug) << "Binding function \"" << *function.symbol();
	auto field = BridgeField { function };
	field.callback = std::move(f);
	m_bindings.push_back(std::move(field));
}

void ska::ScriptBindingAST::queryAST() {
	auto found = m_cacheAst.atOrNull(m_templateName);
	if(found == nullptr) {
		SLOG(LogLevel::Info) << "Building template script " << m_templateName;
		auto file = std::ifstream { m_templateName };
		m_templateScript = m_parser.subParse(m_cacheAst, m_templateName, file);
		SLOG(LogLevel::Info) << "Template script built";
	} else {
		m_templateScript = std::make_unique<ScriptAST>(*found);
	}
}

void ska::ScriptBindingAST::fillConstructorWithBindings(BridgeFunction& constructor) {
	assert(!m_bindings.empty() && "Bridge is empty");

	// A function type declared by the template and used by a bound function (ex : "Hook = function(entity: int) do end",
	// "on = function(event: string, hook: Hook) do end") is declared by the binding first : the bound functions refer to it.
	// It is only a type : calling it is an error.
	auto boundNames = std::unordered_set<std::string>{};
	for (const auto& binding : m_bindings) {
		boundNames.insert(binding.name());
	}
	auto types = std::vector<BridgeField>{};
	for (const auto& binding : m_bindings) {
		const auto& functionType = binding.symbolTable->symbol()->type();
		for (std::size_t index = 0; index < functionType.size(); index++) {
			const auto& type = functionType[index];
			const auto* table = type.symbolTable();
			if (type != ExpressionType::FUNCTION || table == nullptr || table->symbol() == nullptr) {
				continue;
			}
			const auto typeName = table->symbol()->name();
			if (typeName.empty() || !boundNames.insert(typeName).second) {
				continue;
			}
			auto field = BridgeField{ *table };
			field.callback = [typeName](std::vector<NodeValue>) -> NodeValue {
				throw std::runtime_error("\"" + typeName + "\" is a function type of a native module : it cannot be called");
			};
			types.push_back(std::move(field));
		}
	}
	for (auto& type : types) {
		constructor.bindField(std::move(type));
	}

	for (auto& binding : m_bindings) {
		constructor.bindField(std::move(binding));
	}
	m_bindings = {};
}

ska::ASTNodePtr ska::ScriptBindingAST::buildFunctionsAST(BridgeFunction& constructor) {
	SLOG(LogLevel::Info) << "Building script " << m_scriptAst.name() << " from bridge";
	SLOG(LogLevel::Info) << "Current constructor is : " << constructor.name();
	fillConstructorWithBindings(constructor);

	auto result = std::vector<ASTNodePtr> {};

	if (constructor.isVoid()) {
		result = m_functionBuilder.makeFieldList(m_scriptAst, constructor);
	} else {
		result.push_back(m_functionBuilder.makeFunction(m_scriptAst, constructor));
	}
	return m_functionBuilder.makeScript(m_scriptAst, std::move(result));
}
