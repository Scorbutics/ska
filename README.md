[![Build status](https://ci.appveyor.com/api/projects/status/hxnab81os4noj270?svg=true)](https://ci.appveyor.com/project/Scorbutics/ska)

# ska engine
ska engine, a light and basic 2D engine to build simple games.

ska uses **SDL2**, with **SDL2_image**, **SDL2_ttf**, **SDL2_mixer**, all under _zlib_ license.
ska also uses **SDL2_giflib_sa**, an unofficial extension based on SDL in order to provide GIF animation support (credits : drx a.k.a. Cédric Van Den Bussche, see more at https://github.com/theMealena/SDL2_giflib_sa) under _LGPL3_ license.

For the physics, ska uses **Chipmunk2D** under the _MIT_ license.

# Scripting
Games are scripted with [skalang](https://github.com/Scorbutics/skalang) (`.miniska` scripts), built with the engine (`external/skalang`).

* A script declares a `run` function, called with the script arguments : `run = function(args: string[]) do ... end`
* Scripts are started by the script triggers of the engine (map events, tiles, characters : `ScriptRefreshSystem`), through the `ScriptRunner` interface. The `ska_scripting` module provides the runner : the script VM (`ScriptVM`).
* A script waits without blocking the game : a native function suspends it (ex : `Engine.wait(500)`, a movement, a message) and the VM resumes it when the action is over. All the scripts share their modules (a module is run once).
* Engine functions : `Engine = import "bind:ska.engine"` (`ska_scripting/src/scripts/ska.engine.miniska`). The skalang standard library modules are available too (`bind:std.native.io.log`, `bind:std.native.math`). A game adds its own functions (modules based on `lang::Module`, with a script template deployed in the `std` directory next to the executables).

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
