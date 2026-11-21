# skalang

![Example](https://github.com/Scorbutics/skalang/blob/master/doc/skalang_script_example.gif)

## Motivation
### Why making another 'scripting' language ? Isn't Python / Ruby / TypeScript / LUA / AngelScript enough ?
Honestly, it all started by the requirement of my other project of builing a 2D game engine (named 'ska').
I needed a robust and fast developing scripting language with a C++ binding, and I wanted it to have some uncommon features, which are listed below.
 * Mainly, Python Ruby and LUA are (designed as) dynamic typed languages, and I just do not like it.
 * In my opinion, TypeScript is an awesome language in term of flexibility and compilation features. It also supports traits natively with its "interfaces", which allows duck typing.
I have no argument against the use of TypeScript, despite the lack of well-known implementations of C++ binding ?
 * AngelScript seems to have good features regarding static typing, but I do not want to use a strict C++-like syntax.

I wanted a simple but coherent language.

## Features
 * Static and strong typing : no implicit conversion. A conversion is explicit, with the converter `:` (`"size : " + (count: string)`, `(3: float)`). A variable keeps its type.
 * Has a compilation phase (to bytecode), that includes type and semantic checks
 * Huge use of type inference
 * Variable-based syntax : functions are variables, import are variables, variables... are variables ;)
 * Object paradigm, without classes (uses a prototyping approach), without polymorphism (uses the function compatibility feature)
 * Only "one way to do a thing", as Python philosophy
 * No "null" value or uninitialized variables (memory safety)
 * Functions are closures : they capture the variables of the functions enclosing them, and each function call has its own variables (recursion is supported)
 * No manual memory management : reference counting, with a cycle collector for the reference cycles (closures capturing each other, objects stored in variables captured by their own methods...)
 * Includes a C++ binding
 * Coroutines : a native (C++) function can suspend the script calling it, the host program resumes it later (ex : a game "wait" or "message" command)

## Syntax
The syntax is a cross between Ruby and Javascript.
It can looks a little weird at first, but it is VERY consistent.
For example, syntax follows some major rules on its use of symbols and keywords :
 * ':' is for the typing system, it explicitly specify a type for an expression. Double it ( '::' ) and it refers to a script namespace.
 * '{' and '}' are for object instanciation (and is used after the 'return' keyword) only
 * 'do' and 'end' are for statement blocks (functions, loops, filters... For an 'if', the 'do' after the condition is optional)
 * '(' and ')' have two uses, one for grouping expression (as in mathematics), and the other for function parameters definition and call
 * '|' is a pipe operator, in the same way as you can find in frameworks like Angular. It applies a lambda function on a collection.
 * '[' and ']' are used for array declaration and use
 * '#' starts a comment, until the end of the line

## Code examples
 * Basic parameters use

```ruby
IOLog = import "bind:std.native.io.log"
PathFcty = import "bind:std.native.io.path"

run = function (parameters: string[]) do
	if parameters.size() > 0
		path = PathFcty.Fcty(parameters[0])
		IOLog.print(path.canonical())

		all = []: int
		parameters | (parameter, index) do
			if index > 0
				all = all + (parameter: int + index * index)
			end
		end

		all | (element) do
			IOLog.print((element: string))
		end

		IOLog.print("size : " + (all.size(): string))

	else
		IOLog.print("No argument provided")
	end
end
```

 * Function type compatibility
```ruby
IOLog = import "bind:std.native.io.log"

Default = function() do end

call = function(deferedArray: Default[]) do
	deferedArray | (defered) do
		defered()
	end
end

run = function(parameters: string[]) do
	callback = function() do
		IOLog.print("defer test")
	end

	callback2 = function() do
		IOLog.print("defer test 2")
	end

	call([callback, callback2])
end
```
 * Object creation

 ```ruby
 Fcty = function(x: int, y: int): var do
	return {
		x = x
		y = y
	}
end
 ```

 * Closures : each object has its own state, shared by its methods (here, `a.Get()` returns 11 and `b.Get()` returns 100)

```ruby
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
```

 * Modules : the variables of a script are shared with the scripts importing it. With the script `counter.miniska` :

```ruby
count = 0
increment = function() do
	count = count + 1
end
```

a script importing it sees the modifications done by its functions (here, `Counter.count` is 1 after the call) :

```ruby
Counter = import "counter"
Counter.increment()
```

## Language rules worth knowing
 * A name can only be used after its declaration (no forward reference). Two functions calling each other are written with a function variable assigned afterwards :
```ruby
next = function(i: int): bool do return false end
isEven = function(i: int): bool do
	r = true
	if i > 0
		r = next(i - 1)
	end
	return r
end
isOdd = function(i: int): bool do
	r = false
	if i > 0
		r = isEven(i - 1)
	end
	return r
end
next = isOdd
```
 * Declaring a variable with the name of a variable of an enclosing function (or of the script) assigns this variable. Inside an object literal, a declaration always creates a new field.
 * Inside the body of a function, the name of the function is the function itself (recursion) : it cannot be assigned there.
 * `return` leaves the function right away. In a function without returned value, `return` alone leaves it.
 * Operators are left associative (`6 - 1 - 2` is `3`). `-` before an operand is a negation (`-1`, `10 - -3`, `12 / -x`, `-f()`) : it binds tighter than the other operators.
 * A function parameter is always a new variable of the function : it hides an outer variable with the same name.
 * Booleans can be compared (`==`, `!=`). Logical operators : `&&`, `||` (the second operand is only evaluated when needed), `!`.
 * `%` is the remainder of an integer division. A division (or a remainder) by zero is an error.
 * A number converted to a string is its shortest text (`(1.5: string)` is `"1.5"`, `(3.0: string)` is `"3"`).
 * Loops : `for(i = 0
 i < 10
 i = i + 1) do ... end`, `while condition do ... end`, and the filter `|` on arrays. Conditions : `if ... else if ... else ... end`.
 * A string has a size (`s.size()`) and characters (`s[0]` is a string). A string cannot be modified : a new one is built (`s + "!"`). An index is an `int`.
 * Mathematics : `Math = import "bind:std.native.math"` (`abs`, `min`, `max`, `clamp`, `sqrt`, `pow`, `floor`, `ceil`, `round`... the functions on floats have their own names : `absFloat`, `minFloat`, `maxFloat`).
 * The top level code of a script runs once : the following imports of the script only give its exports.

## Host API (C++) : calling script functions, coroutines
```cpp
auto executor = ska::bytecode::Executor{};
// Runs the top level code of the script (once) and gives its exported "run" function
auto run = interpreter.exported(executor, scriptIndex, scriptCache, "run");
// Calls it with its arguments, until it returns or is suspended
auto coroutine = interpreter.start(executor, scriptCache, run, { ska::NodeValue{ 3L } });
// In a native function : interpreter.suspend() suspends the calling script once the native function returned
while (coroutine.suspended()) {
	// ... later (ex : next game frame), with the value returned by the native function that suspended it
	interpreter.resume(coroutine, ska::NodeValue{ 1L });
}
auto result = coroutine.result();
```
The coroutines started on an executor share its scripts and their variables : each coroutine only has its own call frames and value stack.


## Installation / Use
The project uses CMake to compile and a C++17 (at least) compiler.
It has **no** dependency excluding the C++ standard library, excluding my own skalog and skabase project, used respectively as logging lib and common programming tools.
It has been tested sucessfully on Windows (10), Linux (18.04) and on an Android (5+) based device with the Termux app to have an in-device terminal.

Clone this repository with the external git submodules :
`git clone --recurse-submodules git@github.com:Scorbutics/skalang.git`
Then you just have to start CMake and compile the project from the root folder :
`cmake -S . -B build`
And
`cmake --build build`
(Recently checked with GCC 15 and CMake 4 on Linux.)
As you might notice, the project is split up in several dynamic libraries, which are modules required by the skalang bytecode runner.
To run an example script (it must declare a `run` function, receiving the command line parameters) :
`./build/bin/skalang_bytecode_runner example.miniska`
(If you build with MSVC, you will probably have to add a target folder (Debug / Release / RelWithDebInfo) after the "bin" folder)

### Checking scripts without running them
`skalang_check` compiles scripts (parsing, type checking, bytecode generation) without running them, and prints their errors :
`./build/bin/skalang_check --bind game.dialog=scripts/game.dialog.miniska --wd game --module game/Levels/story.miniska game/Levels/intro.miniska`
 * `--bind <name>=<file>` : the `bind:<name>` imports read this template file (declaring the functions that the host program binds at run time)
 * `--wd <directory>` : directory of the `wd:` imports
 * `--module <file>` : a script only imported by other scripts (the other files must declare `run = function(args: string[]) do ... end`)

In a CMake project embedding skalang, `skalang_check_scripts` (see `cmake/SkalangCheck.cmake`) runs this check during the build :
```cmake
skalang_check_scripts(game_scripts_check
	SCRIPTS ${LEVEL_SCRIPTS}
	MODULES ${CMAKE_SOURCE_DIR}/Levels/story.miniska
	BIND_DIRECTORIES ${CMAKE_SOURCE_DIR}/src/scripts
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
```

### Advice: the environment of development for this C++ project

This is a really complex topic and usually a hot one. While some people are advocating from using vi, emacs... I think this is important to get less friction as possible from using a development environement daily.

The C++ language environment is usually a bit tricky to setup and does not always provide every quality of life tools we can encounter more easily in other ecosystems.

At the time of writing (2020), there were a lot of benefits from using Visual Studio (Community or Professional), because of its native support of the C++ ecosystem. Everything is relatively easy, from autocompletion, intellisense, profiling and debugging.

Now in 2024, I would suggest to go with VSCode instead which benefits from being available on almost any platform.
In order to be efficient, you can install the following extensions:

- CMake Tools Microsoft extension
- C++ TestMate (compatibility with Doctest allows running test suites from UI almost as easily as in a Java ecosystem)
- C/C++ Microsoft extension (for Intellisense)

I also encourage you to move out the "Testing" tab from the left pane in order to be able to see it while debugging.

## Contributing
Currently, I made this project alone.
I do not usually work on my projects with other people, but I am totally open to it !
To contribute, you can either :
- Create an issue in the GitHub issues page that describe what you want to change
- Implement a feature by forking this repo and making a "Pull Request" on GitHub
- Add some documentation in the wiki

## Code architecture
	Blocks
		- Tokenizer and parser (lib skalang)
		- Type building / semantic type checking (lib skalang)
		- Bytecode generation (lib skalang_bytecode_generator)
		- Serialization (lib skalang_bytecode_serializer)
		- Interpretation, by the virtual machine (VM) (lib skalang_bytecode_interpreter)
		- Runner (facultative executable script starter)

	Runtime model (VM)
		- Each function call has its own call frame (local variables and registers). Script level variables are stored in the script.
		- A local variable captured by a nested function lives in a shared cell : closures are function values carrying their captured cells.
		- The exported variables of a script are cells shared with its importers.
		- Memory is managed by reference counting, plus a cycle collector (CPython-like) for the reference cycles.
		- A coroutine owns its call frames and its value stack, swapped in the executor while it runs (one at a time).

## Benchmarks
No benchmarks on the interpreter (VM) have been done for now (I aim to add some to compare runtime executions with other known major scripting languages)
