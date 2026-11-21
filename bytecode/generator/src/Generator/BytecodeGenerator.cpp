#include "Config/LoggerConfigLang.h"
#include "BytecodeGenerator.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

#include "GeneratorOperatorBlock.h"
#include "GeneratorOperatorUnary.h"
#include "GeneratorOperatorVariable.h"
#include "GeneratorOperatorBinary.h"
#include "GeneratorOperatorFunction.h"
#include "GeneratorOperatorReturn.h"
#include "GeneratorOperatorArray.h"
#include "GeneratorOperatorIf.h"
#include "GeneratorOperatorFor.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "GeneratorOperatorScriptObject.h"
#include "GeneratorOperatorImport.h"
#include "GeneratorOperatorFieldAccess.h"
#include "GeneratorOperatorScriptLink.h"
#include "GeneratorOperatorFilter.h"
#include "GeneratorOperatorConverterCall.h"
#include "GeneratorOperatorFactory.h"

#include "GeneratorDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Generator);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Generator)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Generator)

std::vector<std::unique_ptr<ska::bytecode::GeneratorOperatorUnit>> ska::bytecode::Generator::build() {
	auto result = std::vector<std::unique_ptr<GeneratorOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	GeneratorOperatorDeclare<ska::Operator::BLOCK>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::UNARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::LITERAL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::BINARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::AFFECTATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::PARAMETER_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_CALL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::CONVERTER_CALL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::RETURN>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::ARRAY_TYPE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IF>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IF_ELSE>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FOR_LOOP>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::USER_DEFINED_OBJECT>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FIELD_ACCESS>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FILTER>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IMPORT>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::SCRIPT_LINK>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::SCRIPT_OBJECT>(*this, result);

	return result;
}

ska::bytecode::Generator::Generator(const ReservedKeywordsPool& reserved) :
	m_operatorGenerator(build()) {
}

ska::bytecode::InstructionOutput ska::bytecode::Generator::generatePart(GenerationContext node) {
	const auto& operatorNode = node.pointer().op();
	LOG_INFO << "[" << node.scriptName() << "] Generating " << operatorNode << " " << node.pointer();
	auto& builder = m_operatorGenerator[static_cast<std::size_t>(operatorNode)];
	assert(builder != nullptr);
	return builder->generate(node);
}

const ska::bytecode::ScriptGeneration& ska::bytecode::Generator::generate(ScriptCache& cache, ScriptGenerationHelper script) {
	auto scriptGenName = script.name();
	auto index = script.id();
	auto emplaced = cache.emplace(scriptGenName, std::move(script));
	if (!emplaced) {
		throw std::runtime_error("the script " + scriptGenName + " was already emplaced");
	}
	return generate(cache, index);
}

const ska::bytecode::ScriptGeneration& ska::bytecode::Generator::generate(ScriptCache& cache, std::size_t scriptIndex) {
	auto& scriptGen = cache.at(scriptIndex);
	scriptGen.generate(cache, *this);
	for (const auto& script : cache) {
		LOG_DEBUG << "Final generation " << *script << " for script " << script->name();
	}
	return scriptGen;
}
