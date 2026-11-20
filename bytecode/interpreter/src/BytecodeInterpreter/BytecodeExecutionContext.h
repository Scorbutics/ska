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

			bool incInstruction() { return m_current == nullptr ? false : m_current->incInstruction(); }

			NodeValue getCell(const Operand& v) const;

			void pop(NodeValue& dest) { m_out.pop(dest); }
			void pop(NodeValueArrayRaw& dest, long count) { m_out.pop(dest, count); }

			void pushInEnv(const ScriptVariableRef& dest, NodeValue value) { m_out.pushInEnv(dest, std::move(value)); }
			void aliasEnvInCell(const ScriptVariableRef& dest, NodeValue& objectCell) { m_out.aliasEnvInCell(dest, objectCell); }

			void jumpAbsolute(NodeValue& value);
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			NodeValue jumpReturn(NodeValue& returnedValue);

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

			void release(const Operand& dest);

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
