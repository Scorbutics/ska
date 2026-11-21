# ska engine

2D game engine (SDL2, Chipmunk2D), ECS based, scripted with skalang (`external/skalang`). Games (ex: Smash & Dash) embed it as a submodule and are libraries loaded by the `ska_main` launcher.

## Build and tests

```sh
cmake -S . -B build && cmake --build build -j8
build/bin/ska_core_test && build/bin/ska_scripting_test   # also ska_ai_test, ska_gui_test, ska_particle_test, ska_world_test
```

Run the tests from the repository root: `ska_gui_test` loads `resources/Fonts` relative to the working directory (it needs a display: `xvfb-run` without one).

Rerun `cmake -S . -B build` after adding a source file (the sources are globbed). The build also checks the module boundaries and the scripting test scripts.

## Modules

- `ska_core`: ECS (entities, components, systems), events, the `ScriptRunner` interface (`Core/ECS/Basics/Script`).
- `ska_draw`, `ska_gui`, `ska_audio`, `ska_particle`, `ska_ai`, `ska_physic_chipmunk`, `ska_world` (tile maps, chipsets): engine features.
- `ska_script`: `ScriptRefreshSystem`, the script triggers (map events, tiles) giving the scripts to the runner of their file extension.
- `ska_scripting`: the skalang runner (`ScriptVM`) and the engine functions given to the scripts (`EngineModule`, template `src/scripts/ska.engine.miniska`).

## Boundaries (checked at each build, `cmake/CheckBoundaries.cmake`)

- The engine modules know nothing about the scripting: they do not include skalang or `ska_scripting` headers. They start scripts through `ScriptRunner`.
- `ska_scripting` only depends on `ska_core` among the engine modules.
- C++ (ECS systems) owns the per frame work. Scripts own the content and the sequencing: they call verbs (move, wait, message...) and wait for their completion (a native function suspends the script, a completion event resumes it). Do not make scripts poll every frame.

## Scripting

- Scripts are skalang scripts: read `external/skalang/doc/LANGUAGE.md` before writing one. Keep skalang's rules: strong static typing, no implicit conversion, no null value.
- A native module is a `lang::Module` with a template script declaring its functions (`bind:<name>` → `<name>.miniska`). Update the template with the C++ binding: the static check (`skalang_check_scripts`, using `SKA_SCRIPT_TEMPLATES_DIR`) compiles the game scripts against the templates.
- A function that waits (ex: `Engine.wait`) suspends the script (`interpreter.suspend()`), and the VM resumes it when the action is over.
- Entities cross the boundary as handles (`EntityHandles`): never give a raw `EntityId` to a script, never use an `int` from a script as an `EntityId` without `handles.entity(...)`.
- Engine → scripts: `ScriptVM::call<T>` for a value needed right away (no wait allowed), hooks (`declareEvent`, `EngineModule::emit`) for events. Neither can run while a script step runs: calls throw, emits are deferred.
