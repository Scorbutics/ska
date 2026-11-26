#pragma once
#include <memory>

namespace ska {
	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
}
