#pragma once
#include <memory>
#include <vector>
#include <limits>
#include "BytecodeInterpreter/BytecodeScriptExecution.h"
#include "CycleCollector.h"

namespace ska {
	namespace bytecode {
		class ScriptGeneration;

		// Return address of a function called by the host (the start of a coroutine) : returning there ends the execution
		inline const ScriptVariableRef HostReturnAddress { std::numeric_limits<std::size_t>::max(), std::numeric_limits<std::size_t>::max() };

		// Activation record of a function call, or of the top level code of a script.
		struct CallFrame {
			// Local variables (LOCAL operands) of the function call
			PlainMemoryTable locals;
			// Temporaries (REG operands) of the function call (or of the script top level code)
			PlainMemoryTable registers;
			// Called function value : its captured cells are the UPVAL operands
			NodeValue function;
			// Where to go back when the function returns
			ScriptVariableRef returnAddress;
			bool scriptLevel = false;
		};

		// Execution state proper to a coroutine : its call frames and its value stack.
		// The other parts of the interpretation state (the scripts and their variables) are shared by all the coroutines.
		struct ExecutionThread {
			std::vector<CallFrame> frames;
			std::vector<NodeValue> stack;
		};

		// Runtime state of an interpretation : scripts (with their script level variables), the call frames and the value stack.
		class Executor {
		public:
			using ScriptExecutionContainer = std::vector<std::unique_ptr<ScriptExecution>>;

			Executor() = default;
			Executor(Executor&&) = delete;
			Executor(const Executor&) = delete;
			Executor& operator=(Executor&&) = delete;
			Executor& operator=(const Executor&) = delete;
			// Releases the interpretation state, then collects the reference cycles left
			~Executor();

			CycleCollector& collector() { return m_collector; }

			void pop(NodeValue& dest) {
				assert(!stack.empty());
				dest = std::move(stack.back());
				stack.pop_back();
			}

			void pop(NodeValueArrayRaw& dest, long count) {
				assert(stack.size() >= static_cast<std::size_t>(count));
				for (auto i = 0; i < count && stack.size() > 0; i++) {
					dest.push_front(std::move(stack.back()));
					stack.pop_back();
				}
			}

			bool stackEmpty() const { return stack.empty(); }

			template <class ... Items>
			void push(Items&& ... items) {
				(pushIfNotEmpty(std::forward<decltype(items)>(items)), ...);
			}

			// Pushes a cell reference, even if the cell is still empty (ex : a local function capturing itself)
			void pushCell(NodeValue cellReference) {
				assert(cellReference.isCellReference());
				stack.push_back(std::move(cellReference));
			}

			NodeValue variable(std::size_t scriptIndex) const {
				return scripts[scriptIndex]->lastVariable();
			}

			// Frame of the top level code of a script (registers only)
			void enterScriptLevel();
			void leaveScriptLevel();

			// Frame of a function call
			void call(NodeValue function, ScriptVariableRef returnAddress);
			ScriptVariableRef returnFromCall();

			// Value of an operand (a cell reference is resolved to the value of the cell)
			NodeValue resolveValue(const Operand& operand, const ScriptExecution& current);

			// Makes a local variable a cell (the variable is captured by a closure)
			void makeCell(const Operand& local);
			// Cell reference of a captured variable : a local variable made a cell, or an upvalue of the current closure
			NodeValue cellReference(const Operand& operand);

			template<class T>
			void set(const Operand& dest, ScriptExecution& current, T&& src) {
				switch (dest.type()) {
				case OperandType::LOCAL:
					frame().locals.push(index(dest), std::forward<T>(src));
					break;
				case OperandType::REG:
					frame().registers.push(index(dest), std::forward<T>(src));
					break;
				case OperandType::UPVAL:
					*upvalue(index(dest)) = std::forward<T>(src);
					break;
				case OperandType::SELF:
					throw std::runtime_error("invalid bytecode : the running function cannot be assigned");
				default:
					scriptFromOperand(dest, current).set(dest, std::forward<T>(src));
					break;
				}
			}

			template <class T>
			const T& get(const Operand& operand, const ScriptExecution& current) {
				switch (operand.type()) {
				case OperandType::LOCAL:
					return frame().locals[index(operand)].template nodeval<T>();
				case OperandType::REG:
					return frame().registers[index(operand)].template nodeval<T>();
				case OperandType::UPVAL:
					return static_cast<const NodeValue&>(*upvalue(index(operand))).template nodeval<T>();
				case OperandType::SELF:
					return frame().function.template nodeval<T>();
				default:
					return scriptFromOperand(operand, current).get<T>(operand);
				}
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

			// Exchanges the current call frames and value stack with the ones of a coroutine
			void swapThread(ExecutionThread& thread) {
				std::swap(frames, thread.frames);
				std::swap(stack, thread.stack);
			}

			// Coroutine running on this executor (one at a time)
			bool coroutineRunning() const { return m_coroutineRunning; }
			void setCoroutineRunning(bool running) { m_coroutineRunning = running; }

			ScriptExecution& scriptOrThrow(std::size_t scriptIndex) {
				if (scriptIndex >= scripts.size() || scripts[scriptIndex] == nullptr) {
					throw std::runtime_error("not a valid script at index " + std::to_string(scriptIndex));
				}
				return *scripts[scriptIndex];
			}

		private:
			static std::size_t index(const Operand& operand) { return operand.as<ScriptVariableRef>().variable; }
			CallFrame& frame();
			const CallFrame& frame() const;
			const NodeValue::Cell& upvalue(std::size_t upvalueIndex) const;

			ScriptExecution& scriptFromOperand(const Operand& v, ScriptExecution& current);
			const ScriptExecution& scriptFromOperand(const Operand& v, const ScriptExecution& current);

			std::vector<CallFrame> frames;
			ScriptExecutionContainer scripts;
			std::vector<NodeValue> stack;
			bool m_coroutineRunning = false;
			// Declared last : destroyed after the interpretation state was released
			CycleCollector m_collector;

			void pushIfNotEmpty(NodeValue value) {
				if (!value.empty()) {
					stack.push_back(std::move(value));
				}
			}
		};
	}
}
