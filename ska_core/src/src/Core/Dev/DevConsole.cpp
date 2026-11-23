#include "Core/__internalConfig/LoggerConfig.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include "DevConsole.h"
#include "Core/State/StateHolder.h"
#include "Core/State/StateBase.h"
#include "Core/Inputs/KeyInput.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/SDLLibrary.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Exceptions/TerminateProcessException.h"
#include "Core/Exceptions/ExceptionTrigger.h"

#if defined(SKA_DEV_CONSOLE) && !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#define SKA_DEV_CONSOLE_SOCKET 1
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(__GNUG__)
#include <cxxabi.h>
#endif

namespace ska {
	struct DevConsoleLog;
}
SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::DevConsoleLog);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::DevConsoleLog)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::DevConsoleLog)

struct ska::DevConsole::Connection {
	int fd = -1;
	std::size_t id = 0;
	std::string input;
	bool waiting = false;
};

namespace {
	std::string JsonString(const std::string& text) {
		auto out = std::string{ "\"" };
		for (const auto c : text) {
			switch (c) {
			case '"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20) {
					char buffer[8];
					std::snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned char>(c));
					out += buffer;
				} else {
					out += c;
				}
			}
		}
		return out + "\"";
	}

	std::vector<std::string> Split(const std::string& line, char separator) {
		auto parts = std::vector<std::string>{};
		auto current = std::string{};
		for (const auto c : line) {
			if (c == separator) {
				parts.push_back(std::move(current));
				current.clear();
			} else {
				current += c;
			}
		}
		parts.push_back(std::move(current));
		return parts;
	}

	std::string TypeName(const ska::State& state) {
		const auto* name = typeid(state).name();
		auto result = std::string{ name };
#if defined(__GNUG__)
		auto status = 0;
		auto* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
		if (status == 0 && demangled != nullptr) {
			result = demangled;
		}
		std::free(demangled);
#endif
		if (result.compare(0, 5, "ska::") == 0) {
			result = result.substr(5);
		}
		return result;
	}

	void DescribeStates(const ska::State& state, std::string& out) {
		out += TypeName(state);
		const auto* base = dynamic_cast<const ska::StateBase*>(&state);
		if (base == nullptr) {
			return;
		}
		const auto& subStates = base->subStates();
		if (subStates.empty()) {
			return;
		}
		out += " > ";
		auto first = true;
		for (const auto& subState : subStates) {
			if (!first) {
				out += ", ";
			}
			first = false;
			DescribeStates(*subState, out);
		}
	}
}

ska::DevConsole& ska::DevConsole::get() {
	static DevConsole instance;
	return instance;
}

bool ska::DevConsole::compiled() {
#if defined(SKA_DEV_CONSOLE_SOCKET)
	return true;
#else
	return false;
#endif
}

ska::DevConsole::DevConsole() {
	// Any run can be replayed : its seed is in its logs
	LOG_INFO << "Random seed : " << NumberUtils::seed() << (NumberUtils::seedFromEnvironment() ? " (SKA_SEED)" : " (picked : SKA_SEED=" + std::to_string(NumberUtils::seed()) + " replays it)");
#if defined(SKA_DEV_CONSOLE_SOCKET)
	// SKA_PAUSE_AT : the game pauses by itself at this logic step (a run that replays : the same steps before the requests)
	const auto* pauseAt = std::getenv("SKA_PAUSE_AT");
	if (pauseAt != nullptr) {
		m_pauseAt = std::strtoul(pauseAt, nullptr, 10);
	}
	const auto* path = std::getenv("SKA_CONSOLE");
	if (path == nullptr || *path == '\0') {
		return;
	}
	m_path = path;
	auto address = sockaddr_un{};
	if (m_path.size() >= sizeof(address.sun_path)) {
		LOG_ERROR << "Console : the socket path is too long (" << m_path.size() << " characters, at most " << sizeof(address.sun_path) - 1 << ") : " << m_path;
		return;
	}
	m_listener = ::socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (m_listener < 0) {
		LOG_ERROR << "Console : no socket (" << std::strerror(errno) << ")";
		return;
	}
	address.sun_family = AF_UNIX;
	std::strncpy(address.sun_path, m_path.c_str(), sizeof(address.sun_path) - 1);
	::unlink(m_path.c_str());
	// Only the user who started the game can connect
	const auto mask = ::umask(0077);
	const auto bound = ::bind(m_listener, reinterpret_cast<sockaddr*>(&address), sizeof(address));
	::umask(mask);
	if (bound != 0 || ::listen(m_listener, 8) != 0) {
		LOG_ERROR << "Console : cannot listen on " << m_path << " (" << std::strerror(errno) << ")";
		::close(m_listener);
		m_listener = -1;
		return;
	}
	LOG_INFO << "Console : listening on " << m_path;
#endif
}

ska::DevConsole::~DevConsole() {
#if defined(SKA_DEV_CONSOLE_SOCKET)
	for (auto& connection : m_connections) {
		close(*connection);
	}
	if (m_listener >= 0) {
		::close(m_listener);
		::unlink(m_path.c_str());
	}
#endif
}

bool ska::DevConsole::opened() const {
	return m_listener >= 0;
}

void ska::DevConsole::setEvaluator(DevConsoleEvaluator* evaluator) {
	m_evaluator = evaluator;
}

void ska::DevConsole::removeEvaluator(DevConsoleEvaluator* evaluator) {
	if (m_evaluator == evaluator) {
		m_evaluator = nullptr;
	}
}

void ska::DevConsole::logicStepped() {
	m_steps++;
	if (m_pauseAt != 0 && m_steps == m_pauseAt) {
		LOG_INFO << "Console : paused at step " << m_steps << " (SKA_PAUSE_AT)";
		pause();
	}
	m_waitingSteps.erase(std::remove_if(m_waitingSteps.begin(), m_waitingSteps.end(), [this](WaitingSteps& waiting) {
		if (m_steps >= waiting.target) {
			waiting.reply(DevReply::Value("step " + std::to_string(m_steps)));
			return true;
		}
		return false;
	}), m_waitingSteps.end());
}

void ska::DevConsole::frame(const StateHolder& states) {
	if (!opened()) {
		return;
	}
	const auto* current = states.current();
	m_states.clear();
	if (current != nullptr) {
		DescribeStates(*current, m_states);
	}
	accept();
	for (auto index = std::size_t{ 0 }; index < m_connections.size(); index++) {
		read(*m_connections[index]);
	}
	m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [](const auto& connection) {
		return connection->fd < 0;
	}), m_connections.end());
	if (m_quit) {
		ExceptionTrigger<TerminateProcessException>("Quit asked by the console");
	}
}

void ska::DevConsole::accept() {
#if defined(SKA_DEV_CONSOLE_SOCKET)
	for (;;) {
		const auto fd = ::accept4(m_listener, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if (fd < 0) {
			return;
		}
		auto connection = std::make_unique<Connection>();
		connection->fd = fd;
		connection->id = m_nextConnection++;
		m_connections.push_back(std::move(connection));
	}
#endif
}

void ska::DevConsole::read(Connection& connection) {
#if defined(SKA_DEV_CONSOLE_SOCKET)
	if (connection.fd < 0 || connection.waiting) {
		return;
	}
	char buffer[4096];
	for (;;) {
		const auto count = ::recv(connection.fd, buffer, sizeof(buffer), 0);
		if (count > 0) {
			connection.input.append(buffer, static_cast<std::size_t>(count));
			continue;
		}
		if (count == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
			// Closed before a full request
			if (connection.input.find('\n') == std::string::npos) {
				close(connection);
				return;
			}
		}
		break;
	}
	const auto end = connection.input.find('\n');
	if (end == std::string::npos) {
		return;
	}
	auto line = connection.input.substr(0, end);
	connection.input.clear();
	connection.waiting = true;
	dispatch(line, connection.id);
#else
	(void)connection;
#endif
}

void ska::DevConsole::close(Connection& connection) {
#if defined(SKA_DEV_CONSOLE_SOCKET)
	if (connection.fd >= 0) {
		::close(connection.fd);
		connection.fd = -1;
	}
#else
	(void)connection;
#endif
}

void ska::DevConsole::answer(std::size_t id, unsigned long startStep, const DevReply& reply) {
	auto json = std::stringstream{};
	json << "{\"ok\":" << (reply.ok ? "true" : "false");
	if (reply.ok) {
		json << ",\"value\":" << JsonString(reply.value);
	} else {
		json << ",\"error\":" << JsonString(reply.error);
	}
	for (const auto& [name, value] : reply.fields) {
		json << "," << JsonString(name) << ":" << value;
	}
	json << ",\"frames\":" << (m_steps - startStep) << "}\n";
	const auto text = json.str();

	auto connection = std::find_if(m_connections.begin(), m_connections.end(), [id](const auto& c) { return c->id == id; });
	if (connection == m_connections.end() || (*connection)->fd < 0) {
		return;
	}
#if defined(SKA_DEV_CONSOLE_SOCKET)
	// A short answer to a local socket : sent at once (the client waits for it)
	const auto fd = (*connection)->fd;
	auto flags = ::fcntl(fd, F_GETFL);
	::fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	auto sent = std::size_t{ 0 };
	while (sent < text.size()) {
		const auto count = ::send(fd, text.data() + sent, text.size() - sent, MSG_NOSIGNAL);
		if (count <= 0) {
			break;
		}
		sent += static_cast<std::size_t>(count);
	}
#endif
	close(**connection);
}

void ska::DevConsole::dispatch(const std::string& line, std::size_t id) {
	// The code of "eval" has its new lines as \x1f (unit separator) : one request is one line
	auto parts = Split(line, '\t');
	const auto command = parts[0];
	const auto startStep = m_steps;
	auto reply = [this, id, startStep](DevReply r) { answer(id, startStep, r); };
	LOG_INFO << "Console : " << command;

	if (command == "ping") {
		auto r = DevReply::Value(m_states);
		r.fields.emplace_back("state", JsonString(m_states));
		r.fields.emplace_back("step", std::to_string(m_steps));
		r.fields.emplace_back("paused", m_paused ? "true" : "false");
		r.fields.emplace_back("evaluator", m_evaluator != nullptr ? "true" : "false");
		reply(std::move(r));
	} else if (command == "quit") {
		reply(DevReply::Value("quit"));
		m_quit = true;
	} else if (command == "eval" || command == "run") {
		if (m_evaluator == nullptr) {
			reply(DevReply::Error("no script evaluator in this game (it has no ScriptVM yet : try again once a scene is loaded)"));
		} else if (parts.size() < 2 || parts[1].empty()) {
			reply(DevReply::Error(command + " : nothing to run"));
		} else if (command == "eval") {
			auto code = parts[1];
			std::replace(code.begin(), code.end(), '\x1f', '\n');
			m_evaluator->eval(code, reply);
		} else {
			auto arguments = std::vector<std::string>(parts.begin() + 2, parts.end());
			m_evaluator->run(parts[1], std::move(arguments), reply);
		}
	} else if (command == "shot") {
		if (parts.size() < 2 || parts[1].empty()) {
			reply(DevReply::Error("shot : a file path is needed"));
		} else {
			shot(parts[1], reply);
		}
	} else if (command == "press") {
		const auto steps = parts.size() > 2 ? std::atoi(parts[2].c_str()) : 1;
		if (parts.size() < 2 || !press(parts[1], static_cast<unsigned int>(std::max(1, steps)), reply)) {
			reply(DevReply::Error("press : unknown key \"" + (parts.size() > 1 ? parts[1] : std::string{}) + "\" (SDL key names : Return, Space, Escape, Up, Left, A, 1...)"));
		}
	} else if (command == "pause") {
		pause();
		reply(DevReply::Value("paused"));
	} else if (command == "resume") {
		resume();
		reply(DevReply::Value("resumed"));
	} else if (command == "step") {
		const auto steps = parts.size() > 1 ? std::max(1, std::atoi(parts[1].c_str())) : 1;
		if (!m_paused) {
			reply(DevReply::Error("step : pause first"));
		} else {
			step(static_cast<unsigned int>(steps));
			// Answered once the steps are done
			const auto target = m_steps + static_cast<unsigned long>(steps);
			m_waitingSteps.push_back(WaitingSteps{ target, reply });
		}
	} else {
		reply(DevReply::Error("unknown command \"" + command + "\" (ping, quit, eval, run, shot, press, pause, resume, step)"));
	}
}

void ska::DevConsole::pause() {
	if (!m_paused) {
		m_paused = true;
		TimeUtils::freeze();
	}
}

void ska::DevConsole::resume() {
	if (m_paused) {
		m_paused = false;
		m_stepsToRun = 0;
		TimeUtils::unfreeze();
	}
}

void ska::DevConsole::step(unsigned int steps) {
	m_stepsToRun += steps;
}

bool ska::DevConsole::takeStep() {
	if (m_stepsToRun == 0) {
		return false;
	}
	m_stepsToRun--;
	return true;
}

void ska::DevConsole::shot(std::string path, DevReplier reply) {
	m_shots.push_back(Shot{ std::move(path), std::move(reply) });
}

void ska::DevConsole::takeShots(SDL_Surface* frame, const SurfaceSaver& save, const std::string& readError) {
	auto shots = std::move(m_shots);
	m_shots.clear();
	for (auto& shot : shots) {
		if (frame == nullptr) {
			shot.reply(DevReply::Error("shot : the frame could not be read (" + readError + ")"));
			continue;
		}
		auto error = std::string{};
		if (!save(*frame, shot.path, error)) {
			shot.reply(DevReply::Error("shot : cannot save " + shot.path + " (" + error + ")"));
			continue;
		}
		auto r = DevReply::Value(shot.path);
		r.fields.emplace_back("path", JsonString(shot.path));
		r.fields.emplace_back("w", std::to_string(frame->w));
		r.fields.emplace_back("h", std::to_string(frame->h));
		r.fields.emplace_back("state", JsonString(m_states));
		r.fields.emplace_back("step", std::to_string(m_steps));
		shot.reply(std::move(r));
	}
}

bool ska::DevConsole::press(const std::string& key, unsigned int steps, DevReplier reply) {
	const auto scancode = SDLLibrary::get().getScancodeFromName(key);
	if (scancode <= 0) {
		return false;
	}
	m_keys.push_back(PressedKey{ scancode, steps, false, std::move(reply) });
	return true;
}

void ska::DevConsole::applyInput(KeyInput& keys) {
	for (auto& key : m_keys) {
		if (!key.down) {
			keys.setKeyState(key.scancode, true);
			key.down = true;
		} else if (key.steps > 0) {
			key.steps--;
		}
		if (key.down && key.steps == 0) {
			keys.setKeyState(key.scancode, false);
			if (key.reply) {
				key.reply(DevReply::Value("pressed"));
			}
		}
	}
	m_keys.erase(std::remove_if(m_keys.begin(), m_keys.end(), [](const PressedKey& key) {
		return key.down && key.steps == 0;
	}), m_keys.end());
}
