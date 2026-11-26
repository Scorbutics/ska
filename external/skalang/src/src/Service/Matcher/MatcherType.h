#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	struct Type;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherType {
	public:
		~MatcherType() = default;
		MatcherType(const ReservedKeywordsPool& pool) :
			m_reservedKeywordsPool(pool) {}
	
		ASTNodePtr match(TokenReader& input, std::string* typeStr = nullptr);
		ASTNodePtr match(const Type& input);
	
	private:
		static void malformedType(const Type& input, const std::string& additionalMessage = "");

		const ReservedKeywordsPool& m_reservedKeywordsPool;
	};
}
