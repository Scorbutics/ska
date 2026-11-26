#include "Config/LoggerConfigLang.h"
#include "MatcherFilter.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FilterTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFilter)

ska::ASTNodePtr ska::MatcherFilter::match(ScriptAST& input, ASTNodePtr collectionToFilter) {

	SLOG(ska::LogLevel::Info) << "filter is currently applied on previous expression";

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::FILTER>());
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
	
	auto collectionEvent = FilterTokenEvent{ *collectionToFilter, FilterTokenEventType::INIT_COLLECTION, input };
	m_parser.observable_priority_queue<FilterTokenEvent>::notifyObservers(collectionEvent);

	auto collectionElementIteratorToken = input.reader().match(TokenType::IDENTIFIER);
	
	auto placeholderTypeCollectionNode = ASTFactory::MakeEmptyNode();
	if (collectionToFilter->type().has_value()) {
		placeholderTypeCollectionNode->updateType(collectionToFilter->type().value());
	}
	
	auto collectionElementIteratorNode = ASTFactory::MakeNode<Operator::FILTER_PARAMETER_DECLARATION>(std::move(collectionElementIteratorToken), std::move(placeholderTypeCollectionNode));

	auto indexOfElementNode = ASTFactory::MakeEmptyNode();
	if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>())) {
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
		
		auto indexOfElementToken = input.reader().match(TokenType::IDENTIFIER);
		indexOfElementNode = ASTFactory::MakeNode<Operator::FILTER_PARAMETER_DECLARATION>(std::move(indexOfElementToken), ASTFactory::MakeEmptyNode());
	}

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

	auto filterDeclarationNode = ASTFactory::MakeNode<Operator::FILTER_DECLARATION>(std::move(collectionElementIteratorNode), std::move(indexOfElementNode));
	
	auto declarationEvent = FilterTokenEvent{ *filterDeclarationNode, FilterTokenEventType::DECLARATION, input };
	m_parser.observable_priority_queue<FilterTokenEvent>::notifyObservers(declarationEvent);

	const auto blockStartToken = input.reader().actual();
	if (blockStartToken.type() != TokenType::RANGE) {
		throw std::runtime_error("bad token detected (expected a block start token)");
	}

	auto filterDefinitionNode = m_matcherBlock.match(input);

	SLOG(ska::LogLevel::Info) << "filter application parsed with success";

	auto filterNode = ASTFactory::MakeNode<Operator::FILTER>(std::move(collectionToFilter), std::move(filterDeclarationNode), std::move(filterDefinitionNode));

	auto definitionEvent = FilterTokenEvent{ *filterNode, FilterTokenEventType::DEFINITION, input };
	m_parser.observable_priority_queue<FilterTokenEvent>::notifyObservers(definitionEvent);
	return filterNode;

}
