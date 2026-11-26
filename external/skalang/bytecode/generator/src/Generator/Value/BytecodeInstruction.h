#pragma once
#include <variant>
#include "NodeValue/Type.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	namespace bytecode {

		class Instruction {
		public:
			Instruction(Command command, OperandUse dest, OperandUse left = {}, OperandUse right = {}) :
				m_command(std::move(command)),
				m_dest(std::move(dest)),
				m_left(std::move(left)),
				m_right(std::move(right)) {
			}

			Instruction(Command command, std::vector<OperandUse> operands) :
				m_command(std::move(command)),
				m_dest(operands.size() >= 1 ? std::move(operands[0]) : OperandUse{}),
				m_left(operands.size() >= 2 ? std::move(operands[1]) : OperandUse{}),
				m_right(operands.size() >= 3 ? std::move(operands[2]) : OperandUse{}) {
			}

			Instruction() = default;

			Instruction(Instruction&&) noexcept = default;
			Instruction(const Instruction&) = default;
			Instruction& operator=(Instruction&&) noexcept = default;
			Instruction& operator=(const Instruction&) = default;

			auto command() const { return m_command; }
			const auto& left() const { return m_left; }
			const auto& right() const { return m_right;	}
			const auto& dest() const { return m_dest; }

			auto& left() { return m_left; }
			auto& right() { return m_right; }
			auto& dest() { return m_dest; }

			friend std::ostream& operator<<(std::ostream& stream, const Instruction&);
			friend bool operator==(const Instruction& left, const Instruction& right);
			friend bool operator!=(const Instruction& left, const Instruction& right);
		private:
			Command m_command = Command::NOP;
			OperandUse m_dest;
			OperandUse m_left;
			OperandUse m_right;
		};

		std::ostream& operator<<(std::ostream& stream, const Instruction&);
		bool operator==(const Instruction& left, const Instruction& right);
		bool operator!=(const Instruction& left, const Instruction& right);

	}
}
