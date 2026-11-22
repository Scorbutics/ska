[![Linux](https://github.com/Scorbutics/ska/actions/workflows/linux.yml/badge.svg)](https://github.com/Scorbutics/ska/actions/workflows/linux.yml)

# ska engine
ska engine, a light and basic 2D engine to build simple games.

ska uses **SDL3** (3.4 or later), with **SDL3_image**, **SDL3_ttf**, **SDL3_mixer**, all under _zlib_ license. Their headers are in `external/SDL3`, the libraries are loaded at run time (`libSDL3`, `libSDL3_image`, `libSDL3_ttf`, `libSDL3_mixer`). On Linux : `apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev` ; when SDL3_mixer is not installed, the build makes it from its sources (in the `lib` directory of the build).

The renderer uses the `gpu` render driver of SDL3 when it can (Vulkan, Metal, Direct3D 12), else the best other one ; the environment variable `SDL_RENDER_DRIVER` chooses another one.

For the physics, ska uses **Chipmunk2D** under the _MIT_ license.

# Shaders
Fragment shaders run on the drawings (`RenderEffect::shader`, ex: a sprite of an entity with `GraphicComponent::effect`) or on the whole screen (`Renderer::setScreenShader`, post-processing). They need the `gpu` render driver of SDL3 : with another one, the drawings are done without them (a warning is logged).

* Written in GLSL (`#version 450`), compiled to SPIR-V during the build : `ska_compile_shaders(<target> OUTPUT_DIRECTORY <dir> SOURCES <file.frag>...)` (`cmake/SkaShaders.cmake`, needs `glslangValidator`). Metal and Direct3D 12 builds need the shaders compiled with SDL_shadercross (`.frag.msl`, `.frag.dxil`).
* A shader receives the color of the drawing (`layout(location = 0) in vec4`), its texture coordinates (`location = 1`, `vec2`), the texture (`layout(set = 2, binding = 0) uniform sampler2D`) and its uniforms (`layout(set = 3, binding = 0) uniform`, given with `Shader::setUniforms`, or for one drawing with `RenderEffect::shaderParameters`).
* `ska::Shader shader{ "Shaders/name", true }` loads `Shaders/name.frag.spv` (true : it has uniforms) the first time it is drawn.

# Scripting
Games are scripted with [skalang](https://github.com/Scorbutics/skalang) (`.miniska` scripts), built with the engine (`external/skalang`).

* A script declares a `run` function, called with the script arguments : `run = function(args: string[]) do ... end`
* Scripts are started by the script triggers of the engine (map events, tiles, characters : `ScriptRefreshSystem`), through the `ScriptRunner` interface. The `ska_scripting` module provides the runner : the script VM (`ScriptVM`).
* A script waits without blocking the game : a native function suspends it (ex : `Engine.wait(500)`, a movement, a message) and the VM resumes it when the action is over. All the scripts share their modules (a module is run once).
* Engine functions : `Engine = import "bind:ska.engine"` (`ska_scripting/src/scripts/ska.engine.miniska`). The skalang standard library modules are available too (`bind:std.native.io.log`, `bind:std.native.math`). A game adds its own functions (modules based on `lang::Module`, with a script template deployed in the `std` directory next to the executables).
* Entities are given to the scripts as `int` handles (`EntityHandles`) : the handle of a removed entity is not valid anymore (using it is an error, `Engine.exists(entity)` tests it), even when the engine reuses its id. The game modules convert their entity parameters with the same `EntityHandles`.
* Synchronous calls : the engine asks a script for a value with `vm.call<long>("file.miniska", "function", { arguments })` (ex : a formula, a decision). The arguments and the returned type are checked against the function type. The function cannot wait, and the engine calls it between the script steps.
* Hooks : a script registers a function called at each engine event, with the entity concerned : `Engine.on("event", function(entity: int) do ... end)` (returns a hook id for `Engine.removeHook`). The game declares its events (`vm.declareEvent`) and emits them (`engineModule.emit(event, entity)`). A hook runs as a script : it can wait. Emitted during a script step, the hooks start after it.
* Static check : a game checks its scripts during its build with `skalang_check_scripts` (skalang `cmake/SkalangCheck.cmake`), giving `${SKA_SCRIPT_TEMPLATES_DIR}` and its own templates as `BIND_DIRECTORIES`. The scripts are compiled (syntax, types, calls of the bound functions) without running them.

Module boundaries, checked at each build (`cmake/CheckBoundaries.cmake`) : the engine modules know nothing about the scripting (they only use `ScriptRunner`), and `ska_scripting` only depends on `ska_core`.

# Made with ska engine (by me, for now...)
Pokémon Smash & Dash - long term fan game of a Pokémon with real time battles
![Smash and Dash](http://www.coldragon.fr/scorbutics/data/upload/spore_ville_smash_and_dash.png)

https://github.com/Scorbutics/SmashAndDash

Point of attraction - created during 48 secret september 2017 jam
![Point of attraction](https://img.itch.zone/aW1hZ2UvMTc0NTk5LzgxNDgyMy5wbmc=/original/59b2aV.png)

https://scorbutics.itch.io/point-of-attraction

ska Pong - basic Pong game as a beginner tutorial
https://github.com/Scorbutics/SkaPong

# Tutorials

https://github.com/Scorbutics/ska/wiki/Basic-game-example

License noonegivesafuck @2020
