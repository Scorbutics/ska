#pragma once
#include <ostream>
#include <vector>
#include <cassert>
#include "Runtime/Value/PlainMemoryTable.h"
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeInstruction.h"
#include "BytecodeSymbolInfo.h"

namespace ska {
	namespace bytecode {
		class ScriptGeneration;

		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class InstructionOutput {
		public:
			InstructionOutput(InstructionOutput&&) noexcept = default;
			InstructionOutput(const InstructionOutput&) = delete;
			InstructionOutput& operator=(InstructionOutput&&) noexcept = default;
			InstructionOutput& operator=(const InstructionOutput&) = delete;
			~InstructionOutput() = default;

			InstructionOutput(ScriptGeneration script);

			InstructionOutput(Instruction instruction) :
				m_pack(InstructionPack { std::move(instruction) }) {
				m_operand = packAsOperand();
			}

			InstructionOutput() = default;

			InstructionOutput(OperandUse operand) {
				m_operand = std::move(operand);
			}

			void push(OperandUse operand);
			void push(Instruction operand);
			void push(InstructionOutput output);

			bool empty() const {
				return m_pack.empty() && m_operand.empty();
			}

			auto size() const { return m_pack.size();	}

			Operand name() const { return m_pack.empty() ? m_operand : m_pack[0].dest(); }

			auto begin() const { return m_pack.begin(); }
			auto end() const { return m_pack.end(); }

			auto begin() { return m_pack.begin(); }
			auto end() { return m_pack.end(); }

			const Instruction* back() const { return m_pack.empty() ? nullptr : &m_pack.back(); }


			Operand operand() const {	return m_operand.empty() ? packAsOperand() : m_operand;	}

			const Instruction& operator[](std::size_t index) const { assert(index < m_pack.size()); return m_pack[index]; }
			Instruction& operator[](std::size_t index) { assert(index < m_pack.size()); return m_pack[index]; }

		private:
			friend std::ostream& operator<<(std::ostream& stream, const InstructionOutput&);
			friend bool operator==(const InstructionOutput& lhs, const InstructionOutput& rhs);
			friend bool operator!=(const InstructionOutput& lhs, const InstructionOutput& rhs);

			Operand packAsOperand() const;

			InstructionPack m_pack;
			Operand m_operand;
		};

    std::ostream& operator<<(std::ostream& stream, const InstructionOutput&);
		bool operator==(const InstructionOutput& lhs, const InstructionOutput& rhs);
		bool operator!=(const InstructionOutput& lhs, const InstructionOutput& rhs);
	}
}
