#pragma once

#include <optional>

#include "ActiveObject.h"
#include "LogEntry.h"

namespace ska {

	class LogPayload {
	public:
		LogPayload(std::function<void()> f) : lambda(std::move(f)) {}
		LogPayload(LogEntry e) : entry(std::move(e)){}

		void operator()() {
			if (entry.has_value()) {
				entry.value().consumeTokens();
			} else {
				lambda();
			}
		}

	private:
		std::optional<LogEntry> entry;
		std::function<void()> lambda;
	};

    class LogAsync {
    private:
        ActiveObject<LogPayload> m_commander;
    public:
        void log(LogEntry entry) {
			m_commander.send(LogPayload{ std::move(entry) });
        }
    };
}

