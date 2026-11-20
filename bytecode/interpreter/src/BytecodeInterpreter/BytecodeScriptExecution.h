#pragma once
#include <memory>
#include <unordered_map>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Runtime/Value/NodeValue.h"
#include "Value/InstructionMemoryTable.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {
		class ScriptExecution {
		public:
			ScriptExecution(const GenerationOutput& instructions, std::size_t scriptIndex) :
				instructions(instructions),
				scriptIndex(scriptIndex) {
			}

			ScriptExecution(ScriptExecution&&) = default;
			ScriptExecution(const ScriptExecution&) = delete;
			ScriptExecution& operator=(const ScriptExecution&) = delete;
			ScriptExecution& operator=(ScriptExecution&&) = delete;

			const Instruction& currentInstruction() const {
				assert(instructions.isGenerated(scriptIndex));
				return instructions[scriptIndex][executionPointer];
			}

			bool incInstruction() {
				if (scriptIndex >= instructions.size()) { return false; }
				return ++executionPointer < instructions[scriptIndex].size();
			}

			NodeValue getCell(const Operand& v) const;

			void jumpAbsolute(std::size_t value);
			void jumpRelative(long value);

			std::size_t id() const;
			const std::string& name() const;

			ScriptVariableRef getRelativeInstruction(long relativeValue) const {
				return ScriptVariableRef{ executionPointer + relativeValue, scriptIndex };
			}

			auto size() const {
				if (scriptIndex >= instructions.size()) { return static_cast<std::size_t> (0u); }
				return instructions[scriptIndex].size();
			}

			bool idle() const {
				if (scriptIndex >= instructions.size()) { return true; }
				return executionPointer >= instructions[scriptIndex].size();
			}

			template <class T>
			T& get(Operand& v) {
				auto* memory = selectMemory(v);
				if(memory == nullptr) {
					if constexpr (!NodeValue::is_container_of_values<T>() && Operand::is_member_of_values<T>()) {
						return v.as<T>();
					} else {
						throw std::runtime_error("invalid get cell value usage by querying a variable container without a valid value provided");
					}
				}
				auto& nodeValue = (*memory)[v.as<ScriptVariableRef>().variable];
				return nodeValue.nodeval<T>();
			}

			template <class T>
			const T& get(const Operand& v) const {
				auto* memory = selectMemory(v);
				if(memory == nullptr) {
					if constexpr (!NodeValue::is_container_of_values<T>() && Operand::is_member_of_values<T>()) {
						return v.as<T>();
					} else {
						throw std::runtime_error("invalid get cell value usage by querying a variable container without a valid value provided");
					}
				}
				auto& nodeValue = (*memory)[v.as<ScriptVariableRef>().variable];
				return nodeValue.nodeval<T>();
			}

			template <class T>
			void set(const Operand& dest, T&& src) {
				auto* memory = selectMemory(dest);
				if(memory == nullptr) { throw std::runtime_error("invalid bytecode destination cell"); }
				memory->push(dest.as<ScriptVariableRef>().variable, std::forward<T>(src));
			}

			auto index() const { return scriptIndex; }

			// Address of the current instruction
			ScriptVariableRef currentAddress() const {
				return ScriptVariableRef{ executionPointer, scriptIndex };
			}

			NodeValue lastVariable() const;

			const NodeValueArray& exports() const { return m_exportsSection; }
			void setExportsSection(NodeValueArray exportsSection) { m_exportsSection = std::move(exportsSection); }


		private:
			PlainMemoryTable* selectMemory(const Operand& dest);
			const PlainMemoryTable* selectMemory(const Operand& dest) const;

			template<class IN, class OUT>
			static OUT SelectMemoryHelper(IN& self, const Operand& dest) {
				switch (dest.type()) {
				case OperandType::PURE:
				default:
					return nullptr;
				case OperandType::VAR:
					return &self.variables;
				case OperandType::LOCAL:
				case OperandType::REG:
				case OperandType::UPVAL:
					throw std::runtime_error("invalid memory : call frame operands are not stored in the script");
				case OperandType::EMPTY:
					throw std::runtime_error("cannot select empty variable relative memory");
				}
			}

			const GenerationOutput& instructions;
			const std::size_t scriptIndex = 0;
			std::size_t executionPointer = 0;

			NodeValueArray m_exportsSection;
			// Script level variables (VAR operands). Function locals and registers live in call frames.
			PlainMemoryTable variables;

		};
	}
}
