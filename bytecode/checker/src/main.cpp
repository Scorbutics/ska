// skalang_check : compiles scripts (parsing, type checking, bytecode generation) without running them.
// Usage : skalang_check [--run] [--bind <name>=<template file>]... [--wd <directory>] [--module] <file> [[--module] <file>]...
//  - "bind:<name>" imports read the template file declaring the native functions (the host program binds them at run time)
//  - "wd:" imports are relative to the --wd directory (default : the current directory)
//  - a file is a script : it must declare "run = function(args: string[]) do ... end",
//    unless it follows --module (a script only imported by other scripts)
//  - --run : the scripts (not the modules) are run after being compiled : their "run" function is called with no argument.
//    The "bind:" imports read from template files run the default bodies of the templates (fakes). Test scripts use the
//    assertions of "bind:std.native.test" : a failed assertion (or any error) fails the script.
// The exit code is the number of files that failed (capped at 255).
#include <iostream>
#include <fstream>
#include <vector>
#include <cerrno>
#include <cstring>
#include "Config/LoggerConfigLang.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "Service/ScriptNameBuilder.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Value/InterpreterTypes.h"
#include "Base/IO/Files/FileUtils.h"

#include "std/module/io/log.h"
#include "std/module/io/path.h"
#include "std/module/math.h"
#include "std/module/function/parameter.h"
#include "std/module/test.h"

#ifdef _WIN32
#include <direct.h>
#define SKALANG_CHDIR _chdir
#else
#include <unistd.h>
#define SKALANG_CHDIR chdir
#endif

namespace {
	struct FileToCheck {
		std::string path;
		bool module = false;
	};

	void check(const FileToCheck& file, bool run) {
		const auto reservedKeywords = ska::ReservedKeywordsPool{};
		auto typeCrosser = ska::TypeCrosser{};
		auto parser = ska::StatementParser{ reservedKeywords };
		auto typeBuilder = ska::TypeBuilder{ parser, typeCrosser };
		auto typeChecker = ska::SemanticTypeChecker{ parser, typeCrosser };

		auto mainCache = ska::bytecode::ScriptCache{};
		auto generator = ska::bytecode::Generator{ reservedKeywords };
		auto interpreter = ska::bytecode::Interpreter{ parser, generator, reservedKeywords };

		auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>{
			mainCache.astCache, typeBuilder, typeChecker, reservedKeywords, parser, mainCache, interpreter
		};

		auto logModule = ska::lang::IOLogModule(moduleConfiguration);
		auto pathModule = ska::lang::IOPathModule(moduleConfiguration);
		auto mathModule = ska::lang::MathModule(moduleConfiguration);
		auto parameterValues = std::make_shared<ska::NodeValueArrayRaw>();
		auto parameterModule = ska::lang::ParameterModule(moduleConfiguration, parameterValues);
		auto testModule = ska::lang::TestModule(moduleConfiguration);

		// The script is imported (and its "run" function called with the right type) by a generated script that is never run
		auto starter = "CheckedScript = import \"" + ska::FileUtils::getCanonicalPath(file.path) + "\"\n";
		if (!file.module) {
			starter += "CheckedArguments = []: string\nCheckedScript.run(CheckedArguments)\n";
		}
		auto script = ska::bytecode::ScriptGenerationHelper{ mainCache, parser, "main", ska::Tokenizer{ reservedKeywords, starter }.tokenize() };
		auto& generated = generator.generate(mainCache, std::move(script));
		if (run && !file.module) {
			auto executor = interpreter.interpret(generated.id(), mainCache);
		}
	}
}

int main(int argc, char* argv[]) {
	auto files = std::vector<FileToCheck>{};
	auto nextIsModule = false;
	auto run = false;
	for (auto i = 1; i < argc; i++) {
		const auto argument = std::string{ argv[i] };
		if ((argument == "--bind" || argument == "--wd") && i + 1 < argc) {
			const auto value = std::string{ argv[++i] };
			if (argument == "--wd") {
				if (SKALANG_CHDIR(value.c_str()) != 0) {
					std::cerr << "Unable to use the directory \"" << value << "\" : " << std::strerror(errno) << std::endl;
					return 255;
				}
				continue;
			}
			const auto separator = value.find('=');
			if (separator == std::string::npos) {
				std::cerr << "--bind expects <name>=<template file>, got \"" << value << "\"" << std::endl;
				return 255;
			}
			ska::ScriptNameBindingAliases()[value.substr(0, separator)] = ska::FileUtils::getCanonicalPath(value.substr(separator + 1));
		} else if (argument == "--run") {
			run = true;
		} else if (argument == "--module") {
			nextIsModule = true;
		} else if (!argument.empty() && argument[0] == '-') {
			std::cerr << "Unknown option \"" << argument << "\"" << std::endl
				<< "Usage : skalang_check [--run] [--bind <name>=<template file>]... [--wd <directory>] [--module] <file>..." << std::endl;
			return 255;
		} else {
			files.push_back({ ska::FileUtils::getCanonicalPath(argument), nextIsModule });
			nextIsModule = false;
		}
	}

	if (files.empty()) {
		std::cerr << "Usage : skalang_check [--run] [--bind <name>=<template file>]... [--wd <directory>] [--module] <file>..." << std::endl;
		return 255;
	}

	auto failed = 0;
	for (const auto& file : files) {
		if (std::ifstream{ file.path }.fail()) {
			std::cerr << file.path << " : file not found" << std::endl;
			failed++;
			continue;
		}
		try {
			check(file, run);
			if (run && !file.module) {
				std::cout << "ok : " << file.path << std::endl;
			}
		} catch (std::exception& e) {
			const auto message = std::string{ e.what() };
			std::cerr << file.path << " : " << message << std::endl;
			if (message.find("\"run\" of \"CheckedScript\"") != std::string::npos) {
				std::cerr << "\ta script declares \"run = function(args: string[]) do ... end\" (a script only imported by other scripts is checked with --module)" << std::endl;
			}
			if (message.find("unable to open script") != std::string::npos) {
				std::cerr << "\ta \"bind:<name>\" import needs \"--bind <name>=<template file>\"" << std::endl;
			}
			failed++;
		}
	}
	if (failed > 0) {
		std::cerr << failed << " / " << files.size() << " script(s) failed to " << (run ? "compile or run" : "compile") << std::endl;
	}
	return failed > 255 ? 255 : failed;
}
