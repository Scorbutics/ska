#pragma once
#include <set>
#include <unordered_map>
#include <vector>
#include "BytecodeOperand.h"
#include "BytecodeInstructionOutput.h"

namespace ska {
	class ASTNode;
	class ScopedSymbolTable;
	class Symbol;

	namespace bytecode {

		// Generation data of a function : the layout of its call frame, and the variables it captures.
		// - Local variables (parameters included) and registers are numbered from 0 for each function :
		//   they live in the call frame of each function call (LOCAL and REG operands).
		// - A variable of an enclosing function is reached through the cells captured by the closure (UPVAL operands),
		//   resolved the same way as Lua upvalues : each intermediate function also captures it.
		// - A local variable captured by a nested function lives in a cell, created by the function prologue (CELL instruction).
		class FunctionScope {
		public:
			FunctionScope(const ASTNode& functionNode, FunctionScope* parent, std::size_t scriptIndex);

			FunctionScope(FunctionScope&&) = delete;
			FunctionScope(const FunctionScope&) = delete;
			FunctionScope& operator=(FunctionScope&&) = delete;
			FunctionScope& operator=(const FunctionScope&) = delete;
			~FunctionScope() = default;

			FunctionScope* parent() const { return m_parent; }

			// Returns the scope of the function declaring the variable "variableTable", among this function and its enclosing ones.
			// Returns nullptr for a script level variable.
			const FunctionScope* declaringScope(const ScopedSymbolTable& variableTable) const;

			// Operand of a variable declared in this function (LOCAL) or in an enclosing one (UPVAL), as seen from this function
			OperandUse variable(const Symbol& symbol, const FunctionScope& declaring, Cursor position = {});

			Register nextRegister();

			// Instructions starting the function : make a cell of each captured local variable
			InstructionOutput prologue() const;

			// Cells to capture when creating the closure, as operands of the enclosing function, in upvalue order
			const std::vector<Operand>& upvalueSources() const { return m_upvalueSources; }

			bool isCaptured(const Operand& operand) const;

		private:
			OperandUse local(const Symbol& symbol, Cursor position = {});
			Operand capturedLocal(const Symbol& symbol);
			const ScopedSymbolTable* symbolTable() const;

			const ASTNode* m_node = nullptr;
			FunctionScope* m_parent = nullptr;
			std::size_t m_script = 0;

			std::unordered_map<const Symbol*, std::size_t> m_locals;
			std::size_t m_register = 0;
			std::set<std::size_t> m_capturedLocals;

			std::unordered_map<const Symbol*, std::size_t> m_upvalueIndexes;
			std::vector<Operand> m_upvalueSources;
		};
	}
}
