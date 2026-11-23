# ska engine — how to reach a running game

Agent tools of the engine (map and decisions: `docs/agent-map.md`, `toolsmith/decisions.md`). Use them before writing an ad-hoc script, a sleep or a screenshot by hand. Linux, Python 3, Xvfb.

- `tools/ska-run up <id> [--cwd DIR] [--env K=V] [--copy SRC:DEST] [--seed N] [--fixed-step] [--pause-at N] [--ready eval] -- <game command>`: runs any ska game in its own run folder (`/tmp/ska-runs-<uid>/<id>`), on its own virtual display, without sound, its logs in the folder (`SKA_LOG_DIR`), no modal box (`SKA_HEADLESS`); returns once the console answers. `ska-run down <id>` (clean quit, prints the exit code), `ps`, `logs <id> [--grep RE] [--tail N] [--strip]`, `path`, `rm`. It never uses the human's display unless `--display`.
- `tools/ska-console [--run ID] <command>`: the development console of a run (`DevConsole`, a Unix socket at `SKA_CONSOLE`). One JSON line back, exit 1 on an error.
  - `eval 'Engine.variable("x")'`: skalang in the live game. One expression answers its value; statements run as a script (they can wait, the answer comes when they end); `eval -` reads the code on stdin. The prelude imports `Engine`, `Dev` and the modules the game registers (`ScriptVM::addConsolePrelude`).
  - `run tools/agent/<script>.miniska args...`: a script file; `Dev.reply(text)` answers. `ska-console scripts` lists the agent scripts (`tools/agent/`: `state`, `entities`).
  - `shot <name>` (PNG of the frame in the run folder), `press <SDL key name> [steps]`, `pause`, `step N`, `resume`, `ping`, `quit`.
- Replay a run exactly: `--seed N --fixed-step --pause-at N` (same seed: same random draws, `NumberUtils::newSeed` for other generators; fixed step: one logic step per frame on a virtual clock; the game pauses itself at step N, before the agent acts).
- Scripts: `bind:ska.dev` (`ska_scripting/src/scripts/ska.dev.miniska`): `reply`, `steps`, `states`, `shot`, `press`, `pause`, `resume`, `step` — for agent scripts only, never game content.
- The console is compiled in every native build (`SKA_DEV_CONSOLE`, OFF on the web and with `-DSKA_DEV_CONSOLE=OFF`) and opens only when `SKA_CONSOLE` is set.
