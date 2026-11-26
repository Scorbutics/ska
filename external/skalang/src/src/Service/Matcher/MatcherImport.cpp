#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "MatcherImport.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

#include "Event/ScriptLinkTokenEvent.h"

#include "Service/ScriptNameBuilder.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherImport)

ska::ASTNodePtr ska::MatcherImport::matchImport(ScriptAST& input) {
	SLOG(ska::LogLevel::Info) << "import expression";
	
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::IMPORT>());

	auto importNodeClass = input.reader().match(TokenType::STRING);
	auto importClassName = ScriptNameDeduce(input.name(), importNodeClass.name());

	auto scriptLinkNode = ASTFactory::MakeNode<Operator::SCRIPT_LINK>(ASTFactory::MakeLogicalNode(Token{ importClassName, TokenType::STRING, importNodeClass.position() }, ASTFactory::MakeEmptyNode()));
	auto scriptLinkEvent = ScriptLinkTokenEvent{ *scriptLinkNode, importClassName, input };
	m_parser.observable_priority_queue<ScriptLinkTokenEvent>::notifyObservers(scriptLinkEvent);

	return scriptLinkNode->type() != ExpressionType::VOID ? std::move(scriptLinkNode) : matchNewImport(input, importNodeClass);
}

ska::ASTNodePtr ska::MatcherImport::matchNewImport(ScriptAST& input, Token importNodeClass) {
	return createNewImport(m_parser, m_parser, m_parser, input, importNodeClass);
}

ska::ASTNodePtr ska::MatcherImport::createNewImport(
		StatementParser& parser,
		observable_priority_queue<ska::BlockTokenEvent>& obsBlock, 
		observable_priority_queue<ska::ImportTokenEvent>& obsImport,
		ScriptAST& input, 
		Token importNodeClass) {
	
	const auto& importedScriptPath = ScriptNameDeduce(input.name(), importNodeClass.name());

	SLOG_STATIC(ska::LogLevel::Info, ska::MatcherImport) << "Create import node script "<< importedScriptPath;

	auto scriptFile = std::ifstream{ importedScriptPath };
	if (scriptFile.fail()) {
		throw std::runtime_error("unable to open script with full-path \"" + importedScriptPath + "\"");
	}

	auto nodeBlock = ska::ASTFactory::MakeNode<ska::Operator::BLOCK>();
	auto startEvent = ska::BlockTokenEvent{ *nodeBlock, ska::BlockTokenEventType::START };
	obsBlock.observable_priority_queue<ska::BlockTokenEvent>::notifyObservers(startEvent);

	input.subParse(parser, importedScriptPath, scriptFile);

	auto importNode = ska::ASTFactory::MakeNode<ska::Operator::IMPORT>(ska::ASTFactory::MakeLogicalNode(Token{importedScriptPath, importNodeClass.type(), importNodeClass.position()}));
	auto importEvent = ska::ImportTokenEvent{ *importNode, input };
	obsImport.observable_priority_queue<ska::ImportTokenEvent>::notifyObservers(importEvent);

	auto endEvent = ska::BlockTokenEvent{ *importNode, ska::BlockTokenEventType::END };
	obsBlock.observable_priority_queue<ska::BlockTokenEvent>::notifyObservers(endEvent);

	return importNode;
}