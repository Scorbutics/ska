#pragma once
#include "Container/sorted_observable.h"

#include "Event/ExpressionTokenEvent.h"
#include "NodeValue/ExpressionType.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/IfElseTokenEvent.h"
#include "Event/ConverterTokenEvent.h"

namespace ska {
	class SymbolTable;
	class StatementParser;
	class TypeCrosser;

	class SemanticTypeChecker :
	public subobserver_priority_queue<VarTokenEvent>,
	public subobserver_priority_queue<FunctionTokenEvent>,
	public subobserver_priority_queue<ArrayTokenEvent>,
	public subobserver_priority_queue<ReturnTokenEvent>,
	public subobserver_priority_queue<IfElseTokenEvent>,
	public subobserver_priority_queue<ConverterTokenEvent> {
	public:
    	SemanticTypeChecker(StatementParser& parser, const TypeCrosser& typeCrosser);
    	~SemanticTypeChecker() = default;
	private:
		static bool childrenHasReturnOnAllControlPath(const ASTNode& node);
		static bool statementHasReturnOnAllControlPath(const ASTNode& node);

    	bool matchArray(const ArrayTokenEvent& token);
    	bool matchVariable(const VarTokenEvent& token); 		
    	bool matchFunction(const FunctionTokenEvent& token);
    	bool matchReturn(const ReturnTokenEvent& token);
		bool matchIfElse(const IfElseTokenEvent& token);
		bool matchConverter(const ConverterTokenEvent& token);

		void matchFunctionCall(const FunctionTokenEvent& token);

		const TypeCrosser& m_typeCrosser;
	};

}
