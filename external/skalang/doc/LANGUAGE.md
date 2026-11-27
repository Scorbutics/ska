# skalang language reference

One page to write correct skalang (`.miniska`) scripts. Every example here compiles. Check a script without running it with `skalang_check` (see the README).

## The rules that differ from other languages

- **Static, strong typing. No implicit conversion.** `"a" + 3` is an error. Convert explicitly with `:` : `"a" + (3: string)`.
- **`:` binds tighter than every operator.** `(a * b: string)` converts only `b`. Write `((a * b): string)`.
- **No null.** A variable always has a value. There is no `null`, `nil`, `None` or optional type.
- **A variable is declared by assigning it** : `x = 1`. There is no `var`, `let`, `const`. It can be reassigned, but it keeps its type (`x = 2.0` after `x = 1` is an error).
- **Functions are variables** : `f = function(a: int): int do ... end`. There is no `function f()`, `def` or `fn`.
- **Blocks are `do ... end`.** Braces `{ }` only build objects (after `return`).
- **One statement per line.** No `;`. An expression cannot continue on the next line (except inside `[ ]`, an `enum` and a `match`).
- **No `switch`, `case`, `in`.** A value chosen by cases is a `match` ; an element of an array is `a.contains(v)`, a key of a map `m.has(k)`.
- **No `++`, `--`, `+=`, `-=`, `*=`, `/=`, ternary `?:`, `break`, `continue`.** Write `i = i + 1`.
- **Strings use double quotes, without escape sequences** (`"\n"` is a backslash and an `n`). A string cannot contain `"`.
- **A name is used after its declaration** (no forward reference, no hoisting).

## Types

| Type | Literals | Notes |
| --- | --- | --- |
| `int` | `0`, `-3` | `/` is the integer division, `%` the remainder. Division by zero is an error. |
| `float` | `1.5`, `-0.25` | |
| `string` | `"text"` | Immutable. `s.size()`, `s[0]` (a `string`). `+` concatenates two strings. |
| `bool` | `true`, `false` | `&&`, `\|\|` (short-circuit), `!`. Compared with `==` and `!=`. |
| arrays | `[1, 2]`, `[]: string` (empty) | `T[]` in a type. `a.size()`, `a[i]`, `a[i] = v`, `a.contains(v)`. An index is an `int`. |
| maps | `["a": 1, "b": 2]`, `[:]: int` (empty) | `T[:]` in a type. String keys : `m["k"] = v`, `m["k"]`, `m.get("k", default)`, `m.has("k")`, `m.keys()` (sorted), `m.size()`. |
| enums | `Status = enum none, poison end` | The name of the enum is the type of its values : `s: Status`. A value : `Status.poison`. |
| objects | `{ x = 1 }` after `return` | Type `var` as a return type. Type `Factory()` for a parameter (see Objects). |
| functions | `function(a: int): int do ... end` | A function type is the name of a function variable with the same signature. |

Conversions with `:` : `int`, `float`, `string` between each other (`("42": int)`, `(1.5: int)`), and `bool` to `string` (`"true"`/`"false"`). A number converted to a string is its shortest text (`(3.0: string)` is `"3"`).

## Statements

```ruby
# A comment, until the end of the line
Log = import "bind:std.native.io.log"

run = function(args: string[]) do
	count = 0
	count = count + 1

	if count > 1
		Log.print("many")
	else if count == 1
		Log.print("one")
	else
		Log.print("none")
	end

	i = 0
	while i < 3 do
		i = i + 1
	end

	# The 3 clauses of a for loop are on 3 lines
	for(j = 0
	j < 2
	j = j + 1) do
		Log.print((j: string))
	end

	# "|" runs a block on each element (with its index). The block returns nothing : it is not a "map"
	squares = []: int
	[1, 2, 3] | (value, index) do
		squares = squares + value * value
	end
end
```

- `if` takes an optional `do` after its condition. `else if` (two words).
- `return` leaves the function immediately. `return` alone leaves a function without a returned value.
- A script started by a host or by the runner declares `run = function(args: string[]) do ... end`.

## Functions

```ruby
add = function(a: int, b: int): int do
	return a + b
end

# Without a return type, the function returns nothing
greet = function(name: string) do
end

# Recursion : inside its body, the function is known by its name
fact = function(n: int): int do
	if n <= 1
		return 1
	end
	return n * fact(n - 1)
end

# A function type is the name of a function with the same parameters and return type
Callback = function(n: int): int do return 0 end
apply = function(f: Callback, n: int): int do
	return f(n)
end
two = apply(function(k: int): int do return k + 1 end, 1)
```

- A function literal can be written anywhere a value is expected (a call argument, an array element) : it is anonymous.
- Functions are closures : they see and modify the variables of the functions enclosing them. Each call has its own variables.
- A parameter is a new variable : it hides an outer variable with the same name.
- Assigning a name declared in an enclosing function (or the script) assigns that variable, it does not create a new one.
- Two functions calling each other : declare a function variable first, assign it after (see the README, "Language rules").

## Objects (no classes)

An object is built by a factory function returning `{ ... }`. Its functions share the variables of the factory (private state).

```ruby
Counter = function(start: int): var do
	value = start
	return {
		increment = function() do
			value = value + 1
		end
		get = function(): int do
			return value
		end
	}
end

# The type of an object is its factory call : Counter()
twice = function(c: Counter()) do
	c.increment()
	c.increment()
end

total = function(counters: Counter()[]): int do
	sum = 0
	counters | (c) do
		sum = sum + c.get()
	end
	return sum
end

a = Counter(1)
twice(a)
n = total([a, Counter(10)])
```

- Fields are read and assigned with `.` : `p.x`, `p.x = 7`. The fields of an object literal are separated by new lines.
- Objects and arrays are shared, not copied : a function receiving an array or an object modifies the caller's one.
- Arrays : `a = a + v` adds an element, `a = a + [v, w]` adds the elements of another array.
- `a.contains(v)` : `true` when an element is equal to `v` (`==`). The elements are `int`, `float`, `string`, `bool` or values of an enum.
- An array (or a map) literal can be written on several lines : a line break is allowed after `[`, after a `,` and before `]`.

## Maps

A map associates string keys to values of one type. Reading a missing key is an error (no null value) : check it with `has`, or read it with `get` and a default value.

```ruby
# A literal : its type comes from its values (all of one type), the keys are strings
prices = [
	"potion": 300,
	"ball": 200
]
cost = prices.get("antidote", 100)

stock = [:]: int
stock["potion"] = 3
stock["ball"] = 10
if stock.has("potion")
	stock["potion"] = stock["potion"] - 1
end

total = function(items: int[:]): int do
	sum = 0
	items.keys() | (name) do
		sum = sum + items[name]
	end
	return sum
end
n = total(stock)

# Values of any type : arrays, objects, functions
types = [:]: string[]
types["fire"] = ["charmander"]
types["fire"] = types["fire"] + "vulpix"
```

- A map is shared, as an array : a function receiving a map modifies the caller's one.
- `keys()` gives the keys sorted : the same order at each run.
- A map is created empty (`[:]: T`) or with its entries (`["k": v]`). Maps cannot be compared (`==`).
- `m.get(key, default)` : the value of the key, or `default` (of the type of the values) when the map has no such key.
- The key of an entry of a literal ends at its `:` : a conversion in a key is written in parentheses (`[(n: string): 1]`).

## Enums

An enum is a type with a fixed set of values. A misspelled value does not compile, where a misspelled string would silently never match.

```ruby
Status = enum
	none, poison, burn
	paralysis
end

slows = function(s: Status): bool do
	return s == Status.paralysis
end

fromHost = ("burn": Status)
name = (Status.poison: string)
lasting = [Status.poison, Status.burn]
hurts = lasting.contains(fromHost)
```

- Declared as the value of a variable : `Name = enum a, b end`. The values are separated by commas or line breaks.
- `Status.poison` is a value ; `Status` in a type (`s: Status`, `Status[]`, `Status[:]`) is the type of its values.
- Compared with `==` and `!=`, only with the values of the same enum (not with a string : convert explicitly).
- Converted with `:` to a `string` (its name), and from a `string` : a text that is not one of the names is an error when the script runs.
- At runtime a value is its name : a native function gives or takes it as a `string`, converted with `:` on the script side.
- An enum of another script : `Lib.Status.poison`, its type `Lib::Status`.

## Match

`match` chooses a value by cases. It is an expression : it can be assigned, returned, given to a function.

```ruby
Weather = enum sun, rain, snow end

speed = function(w: Weather): int do
	return match w
		when Weather.sun then 100
		when Weather.rain, Weather.snow then 70
	end
end

chance = function(event: string): int do
	return match event
		when "attack", "hit" then 30
		when "hurt" then 40
		else 100
	end
end
```

- `match value`, then one branch a line : `when a, b then result`, then optionally `else result`, then `end`.
- The values of a branch are compared to the matched value with `==`, in order, until one is equal : the result of its branch is the value of the match. Only that result is evaluated.
- The matched value is an `int`, a `float`, a `string`, a `bool` or a value of an enum ; the values of the branches have its type.
- The results of the branches have one type. A result is an expression (not statements).
- `else` (the last branch) is needed, unless the branches name every value of an enum.

## Scripts and modules

```ruby
Log = import "bind:std.native.io.log"   # native module (bound by the host program, or the standard library)
Math = import "bind:std.native.math"
Geo = import "point"                     # point.miniska, next to this script
Story = import "wd:Levels/story"         # relative to the working directory

p = Geo.Point(3, 4)
# A type declared by another script : Script::Name
dist = function(q: Geo::Point()): int do
	return q.x
end
```

- The top level code of a script runs once. Every script importing it shares its variables.
- Standard library : `std.native.io.log` (`print(string)`), `std.native.test` (assertions of test scripts : `check`, `equal`, `fail`), `std.native.math` (`abs`, `min`, `max`, `clamp`, `sqrt`, `pow`, `floor`, `ceil`, `round`, and `absFloat`, `minFloat`, `maxFloat` on floats).
- A host program declares its native functions in a template script (`bind:<name>` → `<name>.miniska`) : read it to know the functions available and their types.
- A template can declare a function type taken by a native function (`Hook = function(entity: int) do end`, `on = function(event: string, handler: Hook) : int do ... end`) : pass it a function with the same parameters and return type, named or anonymous. The host calls it later. The type itself (`Module.Hook`) cannot be called.

## Common errors

| Message | Cause |
| --- | --- |
| `Unable to use operator "+" on types "string" and "int"` | Missing conversion : `(x: string)`. Remember `:` binds tighter than operators. |
| `Unable to use operator "=" on types "int" and "float"` | A variable (or parameter, or array cell) keeps its type. |
| `Symbol not found : x` | Not declared yet (or a keyword of another language : the message tells what to write). |
| `bad return type : expected "void"` | A function returning a value needs a return type : `function(): int do`. The block of `\|` returns nothing. |
| `The symbol "x" cannot be declared as a void type` | The value has no type : a function call returning nothing, or an expression cut at the end of the line. |
| `unterminated token` | A string containing `"`. |
| `"x" is not a value of the enum "Status"` | A misspelled value (the message lists the values). |
| `a match needs an "else"` | No branch for some values : add `else`, or name the missing values of the enum. |
