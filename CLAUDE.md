# skalang

Statically typed scripting language (`.miniska`): tokenizer, parser, type checking, bytecode generator, serializer and VM, in C++17. It is embedded in the ska engine (`external/skalang` there).

## Writing scripts

Read [doc/LANGUAGE.md](doc/LANGUAGE.md) before writing or reviewing a script: the syntax differs from Ruby, JavaScript and Python on purpose. Check a script with `skalang_check` (compiles without running) or run it with `skalang_bytecode_runner`.

## Language rules (requirements of the author, do not relax them)

- Static and strong typing. **No implicit conversion**, ever: a conversion is explicit, with `:`.
- **No null value**: a variable always has a value. Do not add null, optional or uninitialized values.
- A variable keeps its type. It can be reassigned with a value of the same type.
- When a language feature is missing, implementing it in the language is welcome, if it keeps these rules.

## Build and tests

```sh
cmake -S . -B build && cmake --build build -j8
cd build/bin && ./skalang_test && ./skalang_bytecode_generator_test && ./skalang_bytecode_serializer_test && ./skalang_bytecode_interpreter_test
```

- Rerun `cmake -S . -B build` after adding a source file: the sources are globbed.
- Run the test executables with a timeout: a code generation bug can make a script loop forever.
- Language features are tested with scripts in `test/bytecode/interpreter/src/460BytecodeInterpreter_language_test.cpp` (`Run` compiles and runs, `Interpret` only parses and type checks).
- Generator tests compare the exact bytecode: when the generated registers change, check that the new code is equivalent before updating them.

## Code map

- `src/src/Service`: tokenizer, `StatementParser` and matchers (`Matcher/`), `ExpressionParser` (shunting yard, priorities in `Container/expression_stack.h`), `SymbolTable`, `TypeBuilder/`, `TypeCrosser/` (allowed operator types), `SemanticTypeChecker`, `LanguageHints.h` (error hints for the habits of other languages).
- `bytecode/generator`: AST to bytecode (`GeneratorOperator*.cpp`, `ComputingOperations/BytecodeTypeConversion.cpp` maps operators and types to commands). `BytecodeCommand.h` lists the commands.
- `bytecode/interpreter`: the VM, one file per command in `Units/`, registered in `BytecodeInterpreter.cpp`. Coroutines: `Interpreter::start/resume/suspend`.
- `bytecode/serializer`: serialized bytecode. **Bump `Serializer::Version`** (with a comment) when the commands or grammar tokens change.
- `std`: native modules (`std/src/std/module`) and their script templates (`std/src/std/scripts`).
- `bytecode/runner`, `bytecode/checker`: the `skalang_bytecode_runner` and `skalang_check` executables. `cmake/SkalangCheck.cmake`: `skalang_check_scripts()` for the projects embedding skalang.

## Adding a feature

1. Parser or type rules, then the generator, then the VM command when needed.
2. A test in the interpreter language tests. Run the 4 test suites.
3. Document it in `doc/LANGUAGE.md` (its examples must compile) and in the README rules.
4. When a mistake of another language's habits gives an unclear error, add a hint in `LanguageHints.h`.
