#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "Service/ScriptNameBuilder.h"

namespace {
	// Removes the aliases added by a test (the registry is global)
	struct AliasesGuard {
		~AliasesGuard() { ska::ScriptNameBindingAliases().clear(); }
	};
}

TEST_CASE("[ScriptNameBuilder] binding aliases") {
	auto guard = AliasesGuard{};
	const auto templateFile = std::string{ SKALANG_TEST_DIR "/src/resources/bind/test.template.miniska" };

	SUBCASE("without alias, a binding is next to the runner") {
		CHECK(ska::ScriptNameDeduce("", "bind:test.template") == ska::ScriptNameRelativeToRunnerDirectory("test.template"));
	}

	SUBCASE("an alias gives the file of a binding") {
		ska::ScriptNameBindingAliases()["test.template"] = templateFile;
		CHECK(ska::ScriptNameDeduce("", "bind:test.template") == ska::FileUtils::getCanonicalPath(templateFile));
		CHECK(ska::ScriptNameDeduce("/any/caller.miniska", "bind:test.template") == ska::FileUtils::getCanonicalPath(templateFile));
	}

	SUBCASE("an alias only changes its own binding") {
		ska::ScriptNameBindingAliases()["test.template"] = templateFile;
		CHECK(ska::ScriptNameDeduce("", "bind:test.other") == ska::ScriptNameRelativeToRunnerDirectory("test.other"));
	}

	SUBCASE("an alias does not change the other namespaces") {
		ska::ScriptNameBindingAliases()["test.template"] = templateFile;
		CHECK(ska::ScriptNameDeduce("", "wd:test.template") == ska::ScriptNameCurrentWorkingDirectory("test.template"));
		CHECK(ska::ScriptNameDeduce("", "std:test.template") == ska::ScriptNameRelativeToStandardLibraryDirectory("test.template"));
	}
}
