#pragma once

#include <cassert>
#include "Generator/Value/BytecodeOperand.h"
#include "Runtime/Value/TokenVariant.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "BytecodeInterpreter/Value/ScriptExecutionOutput.h"
#include "BytecodeScriptExecution.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	class StatementParser;
	namespace bytecode {

		class Generator;
		class ExecutionContext {
		public:
			ExecutionContext(Executor& container, std::size_t scriptIndex, GenerationOutput& instructions);
			ExecutionContext(ExecutionContext& old, std::size_t scriptIndex);

			ExecutionContext(ExecutionContext&&) noexcept = default;
			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool idle() const { return m_current == nullptr ? true : m_current->idle(); }

			const Instruction& currentInstruction() const {
				return m_current->currentInstruction();
			}

			std::size_t currentScriptId() const {
				return m_current->id();
			}

			const std::string& currentScriptName() const {
				return m_current->name();
			}

			ScriptExecution* currentScript() { return m_current; }
			ScriptVariableRef currentAddress() const { return m_current->currentAddress(); }

			// Continues the execution at the given instruction (the next one executed is the following instruction)
			void moveTo(const ScriptVariableRef& address);

			bool incInstruction() { return m_current == nullptr ? false : m_current->incInstruction(); }

			NodeValue getCell(const Operand& v) const;

			void pop(NodeValue& dest) { m_out.pop(dest); }
			void pop(NodeValueArrayRaw& dest, long count) { m_out.pop(dest, count); }

			void pushCell(NodeValue cellReference) { m_out.pushCell(std::move(cellReference)); }
			void makeCell(const Operand& local) { m_out.makeCell(local); }
			NodeValue cellReference(const Operand& operand) const { return m_out.cellReference(operand); }

			// Registers a heap object for the cycle collection / runs a collection if enough objects were registered
			template <class HeapObject>
			void track(const HeapObject& object) { m_out.collector().track(object); }
			void collectCyclesIfNeeded() { m_out.collector().collectIfNeeded(); }

			// Calls a function (new call frame) / returns from the current function call
			void jumpAbsolute(const NodeValue& function);
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			void jumpReturn();

			// Call frame of the script top level code
			void enterScriptLevel() { m_out.enterScriptLevel(); }
			void leaveScriptLevel() { m_out.leaveScriptLevel(); }

			ScriptVariableRef getRelativeInstruction(long relativeValue) const {
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_out.push(std::forward<Items>(items)...);
			}

			template <class T>
			const T& get(const Operand& variable) {
				return m_out.template get<T>(variable, *m_current);
			}

			template <class T>
			void set(const Operand& dest, T&& src) {
				m_out.set(dest, *m_current, std::forward<T>(src));
			}


			ScriptExecutionOutput generateExportedVariables(std::size_t scriptIndex);

			void generate(StatementParser& parser, Generator& generator);
			bool isGenerated(std::size_t scriptIndex) const;

			const NativeFunction& getBinding(const ScriptVariableRef& bindingRef) const;

		private:
			ExecutionContext buildContextForScript(const ScriptVariableRef& value);

			Executor& m_out;
			GenerationOutput& m_in;
			ScriptExecution* m_current = nullptr;
		};
	}
}
