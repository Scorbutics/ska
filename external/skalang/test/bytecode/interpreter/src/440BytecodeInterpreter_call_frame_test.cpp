#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

// Each function call has its own call frame (locals and registers), and closures capture cells.

namespace {
	ska::NodeValue Run(const std::string& progStr) {
		auto [script, data] = Interpret(progStr);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		return data.interpreter->interpret(gen.id(), *data.storage)->variable(gen.id());
	}
}

TEST_CASE("[BytecodeInterpreter] call frame : recursion keeps the locals of each call") {
	const auto result = Run(R"script(
		fact = function(n: int): int do
			before = n
			r = 1
			if n > 1
				r = fact(n - 1)
			end
			return before * r
		end
		out = fact(10)
	)script");
	CHECK(result.nodeval<long>() == 3628800);
}

TEST_CASE("[BytecodeInterpreter] call frame : registers of an expression survive a recursive call") {
	const auto result = Run(R"script(
		fib = function(n: int): int do
			r = n
			if n > 1
				r = fib(n - 1) + fib(n - 2)
			end
			return r
		end
		out = fib(15)
	)script");
	CHECK(result.nodeval<long>() == 610);
}

TEST_CASE("[BytecodeInterpreter] call frame : recursion inside a filter") {
	// The filter iterator, its index and the array length live in the registers of each call :
	// the nested calls (made while iterating) must not alter them
	const auto result = Run(R"script(
		total = function(values: int[], depth: int): int do
			sum = 0
			values | (value, index) do
				sum = sum + value * index
				if depth > 0
					sum = sum + total(values, depth - 1)
				end
			end
			return sum
		end
		out = total([1, 2, 3], 2)
	)script");
	// total(0) = 1*0 + 2*1 + 3*2 = 8, total(d) = 8 + 3 * total(d - 1)
	CHECK(result.nodeval<long>() == 104);
}

TEST_CASE("[BytecodeInterpreter] closure : each factory call has its own captured variables") {
	const auto result = Run(R"script(
		Counter = function(start: int): var do
			value = start
			return {
				Increment = function() do
					value = value + 1
				end
				Get = function(): int do
					return value
				end
			}
		end
		a = Counter(10)
		b = Counter(100)
		a.Increment()
		a.Increment()
		b.Increment()
		out = (a.Get(): string) + "/" + (b.Get(): string)
	)script");
	CHECK(*result.nodeval<ska::StringShared>() == "12/101");
}

TEST_CASE("[BytecodeInterpreter] closure : variable captured through an intermediate function") {
	const auto result = Run(R"script(
		Outer = function(x: int): var do
			Inner = function(y: int): int do
				Deepest = function(z: int): int do
					return x + y + z
				end
				return Deepest(100)
			end
			return {
				Compute = function(): int do
					return Inner(10)
				end
			}
		end
		out = Outer(1).Compute()
	)script");
	CHECK(result.nodeval<long>() == 111);
}

TEST_CASE("[BytecodeInterpreter] closure : local recursive function capturing itself") {
	const auto result = Run(R"script(
		Outer = function(n: int): int do
			Loop = function(i: int): int do
				r = 0
				if i > 0
					r = i + Loop(i - 1)
				end
				return r
			end
			return Loop(n)
		end
		out = Outer(4)
	)script");
	CHECK(result.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] typing : filter iterator over objects keeps the type of the elements") {
	// The iterator "child" is passed to a function expecting the element type
	const auto result = Run(R"script(
		Node = function(depth: int): var do
			kids = [] : Node()
			if depth > 0
				kids = kids + Node(depth - 1)
				kids = kids + Node(depth - 1)
			end
			return {
				children = kids
			}
		end
		count = function(node: Node()): int do
			total = 1
			node.children | (child, index) do
				total = total + count(child)
			end
			return total
		end
		out = count(Node(3))
	)script");
	CHECK(result.nodeval<long>() == 15);
}

TEST_CASE("[BytecodeInterpreter] typing : a method uses a method of another instance of its own factory") {
	// "child.Count()" is typed and generated while "Count" and "Node" are still being declared
	const auto result = Run(R"script(
		Node = function(depth: int): var do
			children = [] : Node()
			if depth > 0
				children = children + Node(depth - 1)
				children = children + Node(depth - 1)
			end
			return {
				Count = function(): int do
					total = 1
					children | (child) do
						total = total + child.Count()
					end
					return total
				end
			}
		end
		out = Node(3).Count()
	)script");
	CHECK(result.nodeval<long>() == 15);
}

TEST_CASE("[BytecodeInterpreter] closure : self recursive local function used after its enclosing function returned") {
	// The function name inside its own body is the running function (no captured cell, so no reference cycle)
	const auto result = Run(R"script(
		Make = function(): var do
			Loop = function(i: int): int do
				r = 0
				if i > 0
					r = i + Loop(i - 1)
				end
				return r
			end
			return {
				run = Loop
			}
		end
		out = Make().run(4)
	)script");
	CHECK(result.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] closure : nested function calling the enclosing recursive function by its name") {
	const auto result = Run(R"script(
		Outer = function(n: int): int do
			Countdown = function(i: int): int do
				Step = function(): int do
					return Countdown(i - 1)
				end
				r = 0
				if i > 0
					r = 1 + Step()
				end
				return r
			end
			return Countdown(n)
		end
		out = Outer(5)
	)script");
	CHECK(result.nodeval<long>() == 5);
}
