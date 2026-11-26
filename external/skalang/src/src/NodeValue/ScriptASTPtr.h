#pragma once
#include <memory>

namespace ska {
	class ScriptAST;

	struct ScriptASTPtr : std::unique_ptr<ScriptAST> {
    	using std::unique_ptr<ScriptAST>::unique_ptr;
    	ScriptASTPtr(std::unique_ptr<ScriptAST> s);
    	ScriptASTPtr(ScriptASTPtr&& s) noexcept = default;
    	ScriptASTPtr& operator=(ScriptASTPtr&& s) = default;
    	ScriptASTPtr() = default;

    	~ScriptASTPtr(); // Implement (empty body) elsewhere
	};
}
