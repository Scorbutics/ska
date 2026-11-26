#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class Symbol;
	
	template<>
	class OperationType<Operator::CONVERTER_CALL> {
	public:
		OperationType(const ASTNode& node) : 
			node(node) {
			assert(node.size() == 2);
		}
	
		inline auto& GetObjectNode() const {
			return node[0];
		}

		inline auto& GetOriginalType() const {
			assert(node[0].type().has_value());
			return node[0].type().value();
		}

		inline auto GetCalledConverterType() const {
			assert(node[1].type().has_value());
			return node[1].type().value();
		}

	private:
		const ASTNode& node;
	};
}
