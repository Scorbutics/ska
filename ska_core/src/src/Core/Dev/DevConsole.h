#pragma once
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct SDL_Surface;

namespace ska {
	class State;
	class StateHolder;
	class KeyInput;

	// Answer to a console request : a text value, or an error. "fields" : more JSON fields (name, raw JSON value)
	struct DevReply {
		bool ok = true;
		std::string value;
		std::string error;
		std::vector<std::pair<std::string, std::string>> fields;

		static DevReply Value(std::string value) { auto reply = DevReply{}; reply.value = std::move(value); return reply; }
		static DevReply Error(std::string error) { auto reply = DevReply{}; reply.ok = false; reply.error = std::move(error); return reply; }
	};
	using DevReplier = std::function<void(DevReply)>;

	// Runs the code sent to the console (the scripting : ska_scripting's ScriptVM). It answers once the code has ended :
	// a script that waits answers after its wait.
	class DevConsoleEvaluator {
	public:
		virtual ~DevConsoleEvaluator() = default;
		// "code" : statements or an expression (its value, as a text, is the answer)
		virtual void eval(const std::string& code, DevReplier reply) = 0;
		// Starts the "run" function of a script file with its arguments
		virtual void run(const std::string& file, std::vector<std::string> arguments, DevReplier reply) = 0;
	};

	/**
	 * \brief Development console : lets a tool (tools/ska-console) look and act inside the running game.
	 *
	 * Compiled with SKA_DEV_CONSOLE (Debug builds), opened only when the environment variable SKA_CONSOLE gives the path of
	 * a Unix socket (tools/ska-run sets it). One request per connection, one line : "<command>\t<argument>\t...", the
	 * answer is one line of JSON. The requests are read by GameCore at the start of each frame, before the logic : no
	 * script step is running then.
	 * Commands : ping, quit, eval <code>, run <file> <arguments...>, shot <path>, press <key> <frames>, pause, resume,
	 * step <frames>. The scripts reach the same verbs through "bind:ska.dev".
	 */
	class DevConsole {
	public:
		static DevConsole& get();

		DevConsole(const DevConsole&) = delete;
		DevConsole& operator=(const DevConsole&) = delete;
		~DevConsole();

		// Compiled in this build (SKA_DEV_CONSOLE)
		static bool compiled();
		// Listening (compiled and SKA_CONSOLE set)
		bool opened() const;

		// GameCore, at the start of each frame : reads the requests and runs them. Throws TerminateProcessException after "quit"
		void frame(const StateHolder& states);
		// GameCore, after each logic step
		void logicStepped();
		// Logic steps since the start
		unsigned long steps() const { return m_steps; }

		// The scripting gives its evaluator (the last one given is used)
		void setEvaluator(DevConsoleEvaluator* evaluator);
		void removeEvaluator(DevConsoleEvaluator* evaluator);

		// Current states, ex : "WorldState > StateMap"
		std::string states() const { return m_states; }

		/* Clock : paused, the logic runs only the steps asked */
		void pause();
		void resume();
		void step(unsigned int steps);
		bool paused() const { return m_paused; }
		// GameCore, when paused : true when a step is to be done (and counts it)
		bool takeStep();

		/* Screenshot : taken by the renderer at the end of the frame */
		void shot(std::string path, DevReplier reply);
		bool shotRequested() const { return !m_shots.empty(); }
		// The renderer, before presenting the frame : saves the frame (a surface read from the screen), answers the requests
		using SurfaceSaver = std::function<bool(SDL_Surface& surface, const std::string& path, std::string& error)>;
		void takeShots(SDL_Surface* frame, const SurfaceSaver& save, const std::string& readError);

		/* Input : a key pressed during some logic steps, as if from the keyboard */
		// Returns false when the key name is unknown (SDL key names : "Return", "Space", "Left", "A"...)
		bool press(const std::string& key, unsigned int steps, DevReplier reply = {});
		// RawInputListener, after reading the SDL events
		void applyInput(KeyInput& keys);

	private:
		DevConsole();

		struct Connection;
		void accept();
		void read(Connection& connection);
		void dispatch(const std::string& line, std::size_t connection);
		void answer(std::size_t connection, unsigned long startStep, const DevReply& reply);
		void close(Connection& connection);

		struct PressedKey {
			int scancode;
			unsigned int steps;
			bool down;
			DevReplier reply;
		};
		struct WaitingSteps {
			unsigned long target;
			DevReplier reply;
		};
		struct Shot {
			std::string path;
			DevReplier reply;
		};

		int m_listener = -1;
		std::string m_path;
		std::vector<std::unique_ptr<Connection>> m_connections;
		std::size_t m_nextConnection = 1;
		DevConsoleEvaluator* m_evaluator = nullptr;
		unsigned long m_steps = 0;
		std::string m_states;
		bool m_paused = false;
		unsigned int m_stepsToRun = 0;
		std::vector<WaitingSteps> m_waitingSteps;
		std::vector<Shot> m_shots;
		std::vector<PressedKey> m_keys;
		bool m_quit = false;
		unsigned long m_pauseAt = 0;
	};
}
