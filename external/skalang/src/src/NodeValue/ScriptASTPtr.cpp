#include "Config/LoggerConfigLang.h"
#include "ScriptASTPtr.h"
#include "ScriptAST.h"

ska::ScriptASTPtr::ScriptASTPtr(std::unique_ptr<ScriptAST> s) : std::unique_ptr<ScriptAST>{std::move(s)} { }

ska::ScriptASTPtr::~ScriptASTPtr() {}
