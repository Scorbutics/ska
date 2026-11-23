# Toolsmith decisions — ska engine

One dated line each, with the map it came from (`docs/agent-map.md`).

- 2026-11-23 — The console evaluates skalang (`eval`, `run <file.miniska>`); the engine verbs (`ping`, `quit`, `shot`, `press`, `pause`/`resume`/`step`) are console commands, also bound for scripts in `bind:ska.dev`. (map grill 1)
- 2026-11-23 — Agent runs target Linux (and the web later); the console transport is a Unix socket. Windows is not a target for agent tooling. (map grill 2)
- 2026-11-23 — `SKA_DEV_CONSOLE` (CMake option) is compiled into every native build by default (Release included: Smash & Dash builds Release by default) and opens only when `SKA_CONSOLE` is set (a Unix socket, mode 0600); the web page leaves it out; `-DSKA_DEV_CONSOLE=OFF` for a shipped build. (map grill 3, revised in the build: asked again)
- 2026-11-23 — Go for every milestone of the map (M1–M3); the deferred cards stay deferred. (review)
