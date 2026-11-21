#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"
#include "Service/ScriptNameBuilder.h"

namespace {
	ska::NodeValue Run(const std::string& progStr) {
		auto [script, data] = Interpret(progStr);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		return data.interpreter->interpret(gen.id(), *data.storage)->variable(gen.id());
	}
}

TEST_CASE("[BytecodeInterpreter] function compatibility : an array of compatible functions is accepted") {
	// The functions of the array only need the same parameters and return types as "Default" (their names do not matter)
	const auto result = Run(R"script(
		Default = function(value: int): int do
			return 0
		end
		applyAll = function(functions: Default[], value: int): int do
			total = 0
			functions | (f) do
				total = total + f(value)
			end
			return total
		end
		double = function(v: int): int do
			return v * 2
		end
		square = function(v: int): int do
			return v * v
		end
		out = applyAll([double, square], 3)
	)script");
	CHECK(result.nodeval<long>() == 15);
}

TEST_CASE("[BytecodeInterpreter] function compatibility : an array of incompatible functions is rejected") {
	constexpr auto progStr = R"script(
		Default = function(value: int): int do
			return 0
		end
		applyAll = function(functions: Default[]) do
		end
		toText = function(v: string): string do
			return v
		end
		applyAll([toText])
	)script";
	CHECK_THROWS_WITH(Run(progStr), doctest::Contains("Unable to use operator \"=\""));
}

TEST_CASE("[BytecodeInterpreter] syntax : an imported script does not need a final line break") {
	const auto result = Run(
		"Imported = import \"" SKALANG_TEST_DIR "/src/resources/no_final_line_break\"\n"
		"out = Imported.value\n");
	CHECK(result.nodeval<long>() == 42);
}

TEST_CASE("[BytecodeInterpreter] syntax : optional \"do\" after an if condition") {
	const auto result = Run(R"script(
		total = 0
		if 1 > 0 do
			total = total + 1
		end
		if 1 > 2 do
			total = total + 10
		else
			total = total + 100
		end
		if total == 101 do total = total + 1000 end
		out = total
	)script");
	CHECK(result.nodeval<long>() == 1101);
}

TEST_CASE("[BytecodeInterpreter] return : leaves the function right away, from any statement") {
	const auto result = Run(R"script(
		early = function(answer: int): string do
			if answer == 1
				return "right"
			end
			return "left " + (answer: string)
		end
		branches = function(answer: int): string do
			if answer == 1
				return "one"
			else
				return "other"
			end
		end
		inLoop = function(values: int[]): int do
			found = 0 - 1
			values | (value, index) do
				if value > 10
					return index
				end
			end
			return found
		end
		out = early(1) + " " + early(0) + " " + branches(1) + " " + branches(2) + " " + (inLoop([1, 5, 12, 30]): string) + " " + (inLoop([1]): string)
	)script");
	CHECK(*result.nodeval<ska::StringShared>() == "right left 0 one other 2 -1");
}

TEST_CASE("[BytecodeInterpreter] return : without value, in a function without returned value") {
	const auto result = Run(R"script(
		count = 0
		increment = function(skip: bool) do
			if skip
				return
			end
			count = count + 1
		end
		increment(false)
		increment(true)
		increment(false)
		out = count
	)script");
	CHECK(result.nodeval<long>() == 2);
}

TEST_CASE("[BytecodeInterpreter] return : without value, in a function with a returned value, is rejected") {
	constexpr auto progStr = R"script(
		f = function(skip: bool): int do
			if skip
				return
			end
			return 1
		end
	)script";
	CHECK_THROWS(Interpret(progStr));
}

TEST_CASE("[BytecodeInterpreter] comments are ignored") {
	const auto result = Run(R"script(
		# Doubles a value
		double = function(v: int): int do # the parameter is an int
			# a comment in a function
			return v * 2 # returned
		end
		#
		out = double(21) # 42
	)script");
	CHECK(result.nodeval<long>() == 42);
}

TEST_CASE("[BytecodeInterpreter] a function parameter hides a script variable with the same name (it does not assign it)") {
	const auto result = Run(R"script(
		counter = 5
		entity = function(name: string): int do
			return 7
		end
		setCounter = function(counter: int) do
			counter = counter + 1
		end
		use = function(entity: int): int do
			return entity * 2
		end
		setCounter(100)
		used = use(entity("a"))
		out = counter * 100 + used + entity("b")
	)script");
	CHECK(result.nodeval<long>() == 5 * 100 + 14 + 7);
}

TEST_CASE("[BytecodeInterpreter] negative numbers and negation") {
	SUBCASE("literals") {
		const auto result = Run(R"script(
			a = -1
			b = 10 - -3
			c = -2 * 3
			d = -2.5 + 1.0
			out = a + b * 100 + c * 10000 + ((d * 2.0): int) * 1000000
		)script");
		CHECK(result.nodeval<long>() == -1 + 13 * 100 - 6 * 10000 - 3 * 1000000);
	}
	SUBCASE("negation of an expression, binding tighter than the other operators") {
		const auto result = Run(R"script(
			x = 6
			f = function(): int do return 4 end
			a = -x
			b = 12 / -x
			c = -x * 2 + -f()
			d = -(x - 10)
			out = a * 1000000 + b * 10000 + c * 100 + d
		)script");
		CHECK(result.nodeval<long>() == -6 * 1000000 - 2 * 10000 - 16 * 100 + 4);
	}
	SUBCASE("subtraction is still a subtraction") {
		const auto result = Run(R"script(
			x = 6
			y = 24 / 4 / 2
			out = (x - 1 - 2) * 100 + y
		)script");
		CHECK(result.nodeval<long>() == 3 * 100 + 3);
	}
}

TEST_CASE("[BytecodeInterpreter] booleans comparison") {
	const auto result = Run(R"script(
		yes = true
		no = 1 > 2
		count = 0
		if yes == true
			count = count + 1
		end
		if no == false
			count = count + 10
		end
		if yes != no
			count = count + 100
		end
		if yes == no
			count = count + 1000
		end
		out = count
	)script");
	CHECK(result.nodeval<long>() == 111);
}

TEST_CASE("[BytecodeInterpreter] modulo, and division by zero is an error") {
	const auto result = Run(R"script(
		out = 17 % 5 * 10 + -7 % 3
	)script");
	CHECK(result.nodeval<long>() == 2 * 10 + (-7 % 3));
	CHECK_THROWS(Run("zero = 0\nout = 1 / zero\n"));
	CHECK_THROWS(Run("zero = 0\nout = 1 % zero\n"));
}

TEST_CASE("[BytecodeInterpreter] logical operators : !, && and || (short-circuit)") {
	const auto result = Run(R"script(
		calls = 0
		count = function(value: bool): bool do
			calls = calls + 1
			return value
		end
		a = true && count(true)
		b = false && count(true)
		c = true || count(false)
		d = false || count(true)
		e = !a
		f = !(1 > 2) && 3 > 2 || false
		g = 1 == 1 && 2 < 1
		out = 0
		if a
			out = out + 1
		end
		if !b
			out = out + 10
		end
		if c
			out = out + 100
		end
		if d
			out = out + 1000
		end
		if !e && f && !g
			out = out + 10000
		end
		out = out + calls * 100000
	)script");
	CHECK(result.nodeval<long>() == 11111 + 2 * 100000);
}

TEST_CASE("[BytecodeInterpreter] else if") {
	const auto result = Run(R"script(
		grade = function(score: int): string do
			if score >= 90
				return "A"
			else if score >= 50
				return "B"
			else if score >= 20 do
				return "C"
			else
				return "D"
			end
		end
		out = grade(95) + grade(60) + grade(25) + grade(3)
	)script");
	CHECK(*result.nodeval<ska::StringShared>() == "ABCD");
}

TEST_CASE("[BytecodeInterpreter] while") {
	const auto result = Run(R"script(
		i = 0
		total = 0
		while i < 5 do
			i = i + 1
			total = total + i
		end
		none = 0
		while false do
			none = 1
		end
		out = total * 10 + none
	)script");
	CHECK(result.nodeval<long>() == 150);
}

TEST_CASE("[BytecodeInterpreter] strings : size and characters") {
	SUBCASE("size and characters") {
		const auto result = Run(R"script(
			s = "hello"
			reversed = ""
			i = s.size() - 1
			while i >= 0 do
				reversed = reversed + s[i]
				i = i - 1
			end
			out = reversed + (s.size(): string)
		)script");
		CHECK(*result.nodeval<ska::StringShared>() == "olleh5");
	}
	SUBCASE("out of range") {
		CHECK_THROWS(Run("s = \"ab\"\nout = s[2]\n"));
	}
	SUBCASE("a string cannot be modified, an index is an int") {
		CHECK_THROWS(Interpret("s = \"ab\"\ns[0] = \"c\"\n"));
		CHECK_THROWS(Interpret("s = \"ab\"\nout = s[\"0\"]\n"));
		CHECK_THROWS(Interpret("a = [1, 2]\nout = a[true]\n"));
	}
}

TEST_CASE("[BytecodeInterpreter] errors : the habits of other languages are explained") {
	const auto hint = [](const std::string& progStr, const char* expected) {
		CHECK_THROWS_WITH(Interpret(progStr), doctest::Contains(expected));
	};
	hint("x = null\n", "there is no null value");
	hint("x = nil\n", "there is no null value");
	hint("x = 1\nif x == 1\nx = 2\nelif x == 2\nx = 3\nend\n", "write \"else if\"");
	hint("x = True\n", "\"true\" and \"false\"");
	hint("x = true and false\n", "\"&&\", \"||\" and \"!\"");
	hint("var x = 1\n", "a variable is declared by assigning it");
	hint("x = \"a\" + 3\n", "no implicit conversion");
	hint("x = 1.0 * 2\n", "no implicit conversion");
	hint("for(i = 0; i < 3; i = i + 1) do\nend\n", "\";\" is not used");
	hint("x = 'a'\n", "double quotes");
	hint("i = 0\ni++\n", "write \"i = i + 1\"");
	hint("i = 0\ni--\n", "write \"i = i - 1\"");
	hint("x = 1\nx += 1\n", "no compound assignment");
	hint("x = 1\nx -= 1\n", "no compound assignment");
	hint("if true {\n}\n", "blocks are \"do ... end\"");
	hint("function f() do\nend\n", "a function is a variable");
	hint("f = function g() do\nend\n", "a function is a variable");
}

TEST_CASE("[BytecodeInterpreter] arrays : a cell can be assigned") {
	SUBCASE("int") {
		const auto result = Run("a = [1, 2]\na[1] = 5\nout = a[0] + a[1]\n");
		CHECK(result.nodeval<long>() == 6);
	}
	SUBCASE("nested") {
		const auto result = Run("a = [[1, 2], [3]]\na[0][1] = 7\nout = a[0][1]\n");
		CHECK(result.nodeval<long>() == 7);
	}
	SUBCASE("an array is shared : a function modifies the array given") {
		const auto result = Run("set = function(values: string[]) do\nvalues[0] = \"b\"\nend\na = [\"a\"]\nset(a)\nout = a[0]\n");
		CHECK(*result.nodeval<ska::StringShared>() == "b");
	}
	SUBCASE("the type of the cell is kept") {
		CHECK_THROWS(Interpret("a = [1]\na[0] = \"b\"\n"));
	}
	SUBCASE("out of range") {
		CHECK_THROWS(Run("a = [1]\na[1] = 2\n"));
	}
}

TEST_CASE("[BytecodeInterpreter] conversions : a boolean converted to a string") {
	const auto result = Run("ok = 1 < 2\nout = (ok: string) + \" \" + (false: string)\n");
	CHECK(*result.nodeval<ska::StringShared>() == "true false");
	CHECK_THROWS(Run("x = (true: int)\n"));
}

TEST_CASE("[BytecodeInterpreter] anonymous functions : a function that is not the value of a variable has no name") {
	constexpr auto apply = "Callback = function(n: int): int do return 0 end\napply = function(f: Callback, n: int): int do\nreturn f(n)\nend\n";
	SUBCASE("call argument at the script level : the assigned variable keeps the returned type") {
		const auto result = Run(std::string{ apply } + "out = apply(function(k: int): int do return k + 1 end, 1)\n");
		CHECK(result.nodeval<long>() == 2);
	}
	SUBCASE("call argument in a function, capturing a variable") {
		const auto result = Run(std::string{ apply } +
			"compute = function(): int do\nbase = 10\nr = apply(function(k: int): int do return k + base end, 5)\nreturn r\nend\nout = compute()\n");
		CHECK(result.nodeval<long>() == 15);
	}
	SUBCASE("array elements") {
		const auto result = Run("fs = [function(k: int): int do return k + 1 end, function(k: int): int do return k * 3 end]\nout = fs[0](1) + fs[1](2)\n");
		CHECK(result.nodeval<long>() == 8);
	}
	SUBCASE("a named function is still known in its body") {
		const auto result = Run("fact = function(n: int): int do\nr = 1\nif n > 1\nr = n * fact(n - 1)\nend\nreturn r\nend\nout = fact(4)\n");
		CHECK(result.nodeval<long>() == 24);
	}
}

TEST_CASE("[BytecodeInterpreter] anonymous functions : an imported script with function literals keeps its exported variables") {
	// The function literals of the script are not variables of the script : the variables declared after them are still exported
	const auto result = Run(
		"Imported = import \"" SKALANG_TEST_DIR "/src/resources/anonymous_functions\"\n"
		"out = Imported.before + Imported.handlers[0](2) + Imported.after\n");
	CHECK(result.nodeval<long>() == 2 + 20 + 42);
}

TEST_CASE("[BytecodeInterpreter] anonymous functions : the value of a field or of an array cell") {
	SUBCASE("object field") {
		const auto result = Run(R"script(
			Op = function(n: int): int do return 0 end
			Holder = function(): var do
				op = function(n: int): int do return n end
				return {
					op = op
				}
			end
			h = Holder()
			h.op = function(n: int): int do return n * 3 end
			out = h.op(5)
		)script");
		CHECK(result.nodeval<long>() == 15);
	}
	SUBCASE("array cell") {
		const auto result = Run("fs = [function(k: int): int do return k end]\nfs[0] = function(k: int): int do return k + 100 end\nout = fs[0](1)\n");
		CHECK(result.nodeval<long>() == 101);
	}
	SUBCASE("a variable assigned with a call taking a function keeps the returned type") {
		// The variable was typed as a function : using it as an int was a type error
		CHECK_NOTHROW(Interpret("Action = function() do end\napply = function(f: Action, n: int): int do\nreturn n\nend\nr = apply(function() do end, 1)\nr = r + 1\n"));
	}
}

TEST_CASE("[BytecodeInterpreter] binding templates : a \"bind:\" import can be read from a template file") {
	// skalang_check compiles scripts this way : the template declares the functions bound by the host program
	struct AliasesGuard {
		~AliasesGuard() { ska::ScriptNameBindingAliases().clear(); }
	} guard;
	ska::ScriptNameBindingAliases()["test.template"] = SKALANG_TEST_DIR "/src/resources/bind/test.template.miniska";

	CHECK_NOTHROW(Interpret("T = import \"bind:test.template\"\nr = T.twice(2) + 1\ns = T.name() + \"!\"\n"));
	CHECK_THROWS_WITH(Interpret("T = import \"bind:test.template\"\nr = T.twice(\"2\")\n"), doctest::Contains("Unable to use operator \"=\""));
	CHECK_THROWS_WITH(Interpret("T = import \"bind:test.template\"\nr = T.unknown()\n"), doctest::Contains("undeclared field"));
}

TEST_CASE("[BytecodeInterpreter] maps : string keys, values of one type") {
	SUBCASE("read, write, size, has, keys (sorted)") {
		const auto result = Run(R"script(
			stock = [:]: int
			stock["potion"] = 3
			stock["ball"] = 10
			stock["potion"] = stock["potion"] + 1
			keys = stock.keys()
			out = keys[0] + "," + keys[1] + "/" + (stock["potion"]: string) + "/" + (stock.size(): string) + "/" + (stock.has("ball"): string) + "/" + (stock.has("key"): string)
		)script");
		CHECK(*result.nodeval<ska::StringShared>() == "ball,potion/4/2/true/false");
	}
	SUBCASE("a map is shared, given to a function, returned by a function") {
		const auto result = Run(R"script(
			fill = function(items: int[:]) do
				items["a"] = 1
			end
			make = function(): int[:] do
				m = [:]: int
				m["b"] = 2
				return m
			end
			m = make()
			fill(m)
			other = m
			other["c"] = 3
			total = 0
			m.keys() | (key) do
				total = total + m[key]
			end
			out = total
		)script");
		CHECK(result.nodeval<long>() == 6);
	}
	SUBCASE("values : arrays, objects, functions") {
		const auto result = Run(R"script(
			P = function(x: int): var do
				return { x = x }
			end
			F = function(n: int): int do return 0 end
			arrays = [:]: string[]
			arrays["fire"] = ["charmander"]
			arrays["fire"] = arrays["fire"] + "vulpix"
			objects = [:]: P()
			objects["a"] = P(7)
			functions = [:]: F
			functions["double"] = function(n: int): int do return n * 2 end
			out = arrays["fire"].size() + objects["a"].x + functions["double"](10)
		)script");
		CHECK(result.nodeval<long>() == 2 + 7 + 20);
	}
	SUBCASE("a missing key is an error") {
		CHECK_THROWS_WITH(Run("m = [:]: int\nout = m[\"missing\"]\n"), doctest::Contains("the map has no key \"missing\""));
	}
}

TEST_CASE("[BytecodeInterpreter] maps : type errors") {
	CHECK_THROWS_WITH(Interpret("m = [:]: int\nm[1] = 2\n"), doctest::Contains("a map key is a \"string\""));
	CHECK_THROWS_WITH(Interpret("m = [:]: int\nm[\"a\"] = \"x\"\n"), doctest::Contains("Unable to use operator \"=\""));
	CHECK_THROWS_WITH(Interpret("m = [:]: int\nm = [:]: string\n"), doctest::Contains("Unable to use operator \"=\""));
	CHECK_THROWS_WITH(Interpret("m = [:]: int\nx = m.unknown()\n"), doctest::Contains("a map has no field \"unknown\""));
	CHECK_THROWS_WITH(Interpret("m = [:]\n"), doctest::Contains("a map is declared with the type of its values"));
	CHECK_THROWS_WITH(Interpret("f = function(x: string[:]) do end\nm = [:]: int\nf(m)\n"), doctest::Contains("Unable to use operator \"=\""));
	CHECK_THROWS(Interpret("m = [:]: int\nn = [:]: int\nb = m == n\n"));
}

TEST_CASE("[BytecodeInterpreter] containers : an object stored in a cell or a field of the same factory") {
	// The store was skipped when the value and the container cell had the same type symbol
	const auto result = Run(R"script(
		P = function(x: int): var do
			return { x = x }
		end
		H = function(): var do
			child = P(1)
			return { child = child }
		end
		arr = [P(1)]
		arr[0] = P(7)
		h = H()
		h.child = P(9)
		out = arr[0].x + h.child.x
	)script");
	CHECK(result.nodeval<long>() == 16);
}

TEST_CASE("[BytecodeInterpreter] built-in fields : on any expression (an array element, a call result)") {
	const auto result = Run("make = function(): int[] do return [1, 2, 3] end\nnested = [[1], [1, 2]]\nout = make().size() * 10 + nested[1].size()\n");
	CHECK(result.nodeval<long>() == 32);
}

TEST_CASE("[BytecodeInterpreter] strings : a string literal with the text of a symbol or a keyword") {
	// "," "end" "do" ended the expression
	const auto result = Run("a = [\"x\", \"y\"]\nout = a[0] + \",\" + a[1] + \"end\" + \"do\" + \"|\"\n");
	CHECK(*result.nodeval<ska::StringShared>() == "x,yenddo|");
}
