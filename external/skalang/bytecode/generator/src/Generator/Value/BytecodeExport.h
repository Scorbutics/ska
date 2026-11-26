#pragma once
#include "BytecodeOperand.h"
#include "Base/Containers/insertion_indexed_map.h"
namespace ska {
	class Symbol;

	namespace bytecode {
		struct ExportSymbol {
			Operand value;
			const Symbol* symbol = nullptr;

			friend bool operator==(const ExportSymbol& lhs, const ExportSymbol& rhs);
			friend bool operator!=(const ExportSymbol& lhs, const ExportSymbol& rhs);
		};

		using ExportSymbolContainer = insertion_indexed_map<const Symbol*, ExportSymbol>;

		bool operator==(const ExportSymbol& lhs, const ExportSymbol& rhs);
		bool operator!=(const ExportSymbol& lhs, const ExportSymbol& rhs);
	}
}
