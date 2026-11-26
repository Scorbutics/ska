#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"

#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

#include "BytecodeInterpreterTest.h"

#include "Runtime/Value/InterpreterTypes.h"
#include "std/module/io/path.h"
#include "std/module/io/log.h"

static ska::bytecode::ScriptGenerationHelper Interpret(BytecodeInterpreterDataTestContainer& data, const std::string& input) {
	readerI->parse(*data.parser);
	return ska::bytecode::ScriptGenerationHelper{*data.storage, *readerI };
}

static ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> BuildModuleConfFromData(BytecodeInterpreterDataTestContainer& data) {
	return ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> { data.storage->astCache, * data.typeBuilder, *data.typeChecker, reservedKeywords, * data.parser, * data.storage, * data.interpreter};
}

TEST_CASE("[BytecodeInterpreter] Binding std : path import only") {
	constexpr auto progStr =
	"PathFcty = import \"bind:std.native.io.path\"\n"
	"path = PathFcty.Fcty(\"" SKALANG_TEST_DIR "\")\n";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = BuildModuleConfFromData(data);
	auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
}

TEST_CASE("[BytecodeInterpreter] Binding std : path + bridge function call") {
	constexpr auto progStr =
	"PathFcty = import \"bind:std.native.io.path\"\n"
	"path = PathFcty.Fcty(\"\")\n"
	"last = path.canonical()\n";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = BuildModuleConfFromData(data);
	auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = *res.nodeval<ska::StringShared>();
	CHECK(cellValue == "");
}

TEST_CASE("[BytecodeInterpreter] Binding std : log + bridge function call") {
	constexpr auto progStr =
		"Logger = import \"bind:std.native.io.log\"\n"
		"Logger.print(\"test63\")\n";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = BuildModuleConfFromData(data);
	auto logModule = ska::lang::IOLogModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
}

TEST_CASE("[BytecodeInterpreter] Binding std log into a function") {
	constexpr auto progStr =
		"run = function(param : string) do\n"
			"Logger = import \"bind:std.native.io.log\"\n"
			"Logger.print(param)\n"
		"end\n"
		"run(\"test83\")\n";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = BuildModuleConfFromData(data);
	auto logModule = ska::lang::IOLogModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
}

#include "std/module/math.h"

TEST_CASE("[BytecodeInterpreter] Binding std : math") {
	constexpr auto progStr =
	"Math = import \"bind:std.native.math\"\n"
	"a = Math.abs(-3) * 100 + Math.min(4, 7) * 10 + Math.max(4, 7)\n"
	"b = Math.clamp(15, 0, 10) + Math.floor(2.7) * 100 + Math.ceil(2.1) * 1000 + Math.round(2.5) * 10000\n"
	"c = Math.sqrt(16.0) + Math.pow(2.0, 3.0) + Math.absFloat(-0.5) + Math.minFloat(1.0, 2.0) + Math.maxFloat(1.0, 2.0)\n"
	"out = (a: string) + \"/\" + (b: string) + \"/\" + (c: string)\n";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);
	auto moduleConfiguration = BuildModuleConfFromData(data);
	auto mathModule = ska::lang::MathModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	CHECK(*res.nodeval<ska::StringShared>() == "347/33210/15.5");
}
