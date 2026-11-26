#pragma once
#include <array>
#include "Container/sorted_observable.h"

#include "NodeValue/Operator.h"
#include "NodeValue/ExpressionType.h"

#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Event/FilterTokenEvent.h"
#include "Event/ConverterTokenEvent.h"

#include "TypeBuildersContainer.h"

namespace ska {
    class StatementParser;
	class TypeCrosser;

    class TypeBuilder : 
	    public subobserver_priority_queue<ExpressionTokenEvent>,
		public subobserver_priority_queue<FunctionTokenEvent>,
		public subobserver_priority_queue<VarTokenEvent>,
		public subobserver_priority_queue<ReturnTokenEvent>,
		public subobserver_priority_queue<ArrayTokenEvent>,
		public subobserver_priority_queue<ScriptLinkTokenEvent>,
        public subobserver_priority_queue<FilterTokenEvent>,
        public subobserver_priority_queue<ConverterTokenEvent> {
        public:
            TypeBuilder(StatementParser& parser, const TypeCrosser& typeCrosser);
            virtual ~TypeBuilder() = default;
            
        private:
            void buildType(ASTNode& node, ScriptAST& script);

            bool matchVariable(VarTokenEvent& token);
            bool matchFunction(FunctionTokenEvent& event);
            bool matchExpression(ExpressionTokenEvent& event);
			bool matchReturn(ReturnTokenEvent& event);
			bool matchArray(ArrayTokenEvent& event);
			bool matchScriptLink(ScriptLinkTokenEvent& event);
            bool matchFilter(FilterTokenEvent& event);
            bool matchConverter(ConverterTokenEvent& event);

			TypeBuildersContainer m_typeBuilder;
    };
    
}
