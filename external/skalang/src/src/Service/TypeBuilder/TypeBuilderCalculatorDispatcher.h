#pragma once

#include "NodeValue/ExpressionType.h"
#include "NodeValue/TypeHierarchy.h"

namespace ska {
    class SymbolTable;
    class ASTNode;

    TypeHierarchy TypeBuilderBuildFromTokenType(const SymbolTable& symbols, const ASTNode& node);
}
