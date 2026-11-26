#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/OperationTypeBinary.h"

namespace ska {
	class TypeCrosser;

    template<>
    struct TypeBuilderOperator<Operator::BINARY> :
		public TypeBuildUnit {
	private:
		using OperateOn = OperationType<Operator::BINARY>;
		const TypeCrosser& m_typeCrosser;
	public:
		TypeBuilderOperator(const TypeCrosser& typeCrosser);
		TypeHierarchy build(const ScriptAST& script, ASTNode& node) override final {
			return build(script, OperateOn{node});
		}
		TypeHierarchy build(const ScriptAST& script, OperateOn node);
	};
}
