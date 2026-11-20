#pragma once
#include <memory>
#include <vector>
#include "BytecodeInterpreter/BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {
		class ScriptGeneration;
		class Executor {
		public:
			using ScriptExecutionContainer = std::vector<std::unique_ptr<ScriptExecution>>;

			void pop(NodeValue& dest) {
				dest = std::move(stack.back());
				/*if (!callstack.empty()) {
					dest.stealEnv(callstack.back());
				}*/
				stack.pop_back();
			}

			void pop(NodeValueArrayRaw& dest, long count) {
				assert(stack.size() >= count);
				for (auto i = 0; i < count && stack.size() > 0; i++) {
					dest.push_front(std::move(stack.back()));
					/*if (!callstack.empty()) {
						dest.front().stealEnv(callstack.back());
					}*/
					stack.pop_back();
				}
			}

			void pushInEnv(const ScriptVariableRef& dest, NodeValue value) {
				callstack.back().captureInEnv(dest, std::move(value));
			}

			// Makes "objectCell" a reference to the captured env variable "dest" of the current call,
			// so that the object field and the captured variable share the same storage.
			// The cell also holds the env, which keeps the referenced env entry alive.
			void aliasEnvInCell(const ScriptVariableRef& dest, NodeValue& objectCell) {
				auto& frame = callstack.back();
				auto* envValue = frame.resolveFromEnv(dest);
				if (envValue == nullptr) {
					throw std::runtime_error("unable to alias an object field to an uncaptured env variable");
				}
				objectCell = NodeValue { frame, envValue };
			}

			template <class ... Items>
			void push(Items&& ... items) {
				(pushIfNotEmpty(std::forward<decltype(items)>(items)), ...);
			}

			NodeValue variable(std::size_t scriptIndex) const {
				return scripts[scriptIndex]->lastVariable();
			}

			void callStack(NodeValue value);
			NodeValue returnCallStack();
			void release(const Operand& dest, ScriptExecution& current);

			NodeValue resolveValue(const Operand& variable, const ScriptExecution& current);

			template<class T>
			void set(const Operand& dest, ScriptExecution& current, T&& src) {
				scriptFromOperand(dest, current).set(dest, src);

				if (callstack.empty() || dest.type() != OperandType::VAR) {
					return;
				}
				callstack.back().overrideInEnv(dest.as<ScriptVariableRef>(), std::forward<T>(src));
			}

			template <class T>
			const T& get(const Operand& variable, const ScriptExecution& current) {
				const auto* value = variable.type() == OperandType::VAR ? useFromCurrentEnv(variable.as<ScriptVariableRef>()) : nullptr;
				return value == nullptr || value->empty() ? scriptFromOperand(variable, current).get<T>(variable) : value->nodeval<T>();
			}

			ScriptExecution& scriptOrNew(const GenerationOutput& instructions, std::size_t scriptIndex) {
				if (scriptIndex >= scripts.size()) {
					scripts.resize(scriptIndex + 1);
				}

				if (scripts[scriptIndex] == nullptr) {
					scripts[scriptIndex] = std::make_unique<ScriptExecution>(instructions, scriptIndex);
				}
				return *scripts[scriptIndex];
			}

			ScriptExecution& scriptOrThrow(std::size_t scriptIndex) {
				if (scriptIndex >= scripts.size() || scripts[scriptIndex] == nullptr) {
					throw std::runtime_error("not a valid script at index " + std::to_string(scriptIndex));
				}
				return *scripts[scriptIndex];
			}
		private:
			ScriptExecution& scriptFromOperand(const Operand& v, ScriptExecution& current);
			const ScriptExecution& scriptFromOperand(const Operand& v, const ScriptExecution& current);
			const NodeValue* useFromCurrentEnv(const ScriptVariableRef& dest) const;

			const ScriptExecution* script(std::size_t scriptIndex) const {
				if (scriptIndex >= scripts.size()) {
					return nullptr;
				}
				return scripts[scriptIndex].get();
			}

			std::vector<NodeValue> callstack;
			ScriptExecutionContainer scripts;
			std::vector<NodeValue> stack;

			void pushIfNotEmpty(NodeValue value) {
				if (!value.empty()) {
					stack.push_back(std::move(value));
				}
			}
		};
	}
}
