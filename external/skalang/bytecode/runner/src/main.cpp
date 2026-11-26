#include <iostream>
#include <fstream>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "BytecodeInterpreter/BytecodeScript.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Serializer/BytecodeSerializer.h"

#include "Runtime/Value/InterpreterTypes.h"

#include "std/module/io/log.h"
#include "std/module/io/path.h"
#include "std/module/math.h"
#include "std/module/function/parameter.h"


static ska::bytecode::ScriptGenerationHelper BasicProgramScriptStarter(ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>& module, char* argv[]) {
	auto scriptFileName = std::string{ argv[1] };
	auto scriptName = scriptFileName.substr(0, scriptFileName.find_last_of('.'));

	const auto scriptStarter = "Script = import \"wd:" + scriptName + "\"\n"
	"ParametersGenerator = import \"bind:std.native.function.parameter\"\n"
	"Script.run(ParametersGenerator.Get(\"" + scriptName + "\"))\n";

	return { module.scriptCache, module.parser, "main", ska::Tokenizer{ module.reservedKeywords, scriptStarter}.tokenize() };
}

static ska::lang::ParameterModule<ska::bytecode::Interpreter> BasicParameterModuleBuilder(ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>& module, ska::NodeValueArray& parameters, int argc, char* argv[]) {
	for(auto i = 2; i < argc; i++) {
		parameters->push_back(std::make_shared<std::string>(argv[i]));
	}

	return ska::lang::ParameterModule(module, parameters);
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cout << "No file name entered. Exiting..." << std::endl;
		return -1;
	}

	auto inputFile = std::ifstream{ std::string{argv[1]} };
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << std::endl;
		return -1;
	} else {
		inputFile.close();
	}

	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto typeCrosser = ska::TypeCrosser{};
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser, typeCrosser };
	auto typeChecker = ska::SemanticTypeChecker {parser, typeCrosser };

	auto mainCache = ska::bytecode::ScriptCache {};
	auto generator = ska::bytecode::Generator{ reservedKeywords };
	auto interpreter = ska::bytecode::Interpreter { parser, generator, reservedKeywords };

	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> {
		mainCache.astCache,
		typeBuilder,
		typeChecker,
		reservedKeywords,
		parser,
		mainCache,
		interpreter
	};

	try {
		auto serializer = ska::bytecode::Serializer{};
		auto logmodule = ska::lang::IOLogModule(moduleConfiguration);
		auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);
		auto mathmodule = ska::lang::MathModule(moduleConfiguration);
		auto parameterValues = std::make_shared<ska::NodeValueArrayRaw>();
		auto parameterModule = BasicParameterModuleBuilder(moduleConfiguration, parameterValues, argc, argv);

		//auto failedToRead = serializer.deserialize(moduleConfiguration.scriptCache, "main", ska::bytecode::DeserializationStrategyType::PerScript(), {"main"});

		auto script = BasicProgramScriptStarter(moduleConfiguration, argv);
		auto& gen = generator.generate(moduleConfiguration.scriptCache, std::move(script));

		//moduleConfiguration.scriptCache.printDebugInfo(std::cout);

		auto interpreted = interpreter.interpret(gen.id(), moduleConfiguration.scriptCache);

		//serializer.serialize(moduleConfiguration.scriptCache, ska::bytecode::SerializationStrategyType::PerScript());


	} catch (std::exception& e) {
		std::cerr << "Error : " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

