#include <iostream>

#include "LoggerImpl.h"
#include "LogEntry.h"

ska::loggerdetail::Logger::Logger() :
	m_logLevel(LogLevel::Debug) {
	m_pattern.emplace(LogLevel::Debug, Tokenizer{ "%10c[%h:%m:%s:%T]%10c[Debug]%8c(%12F l.%4l) %15c%v" });
	m_pattern.emplace(LogLevel::Info, Tokenizer{ "%10c[%h:%m:%s:%T]%11c[Info ]%8c(%12F l.%4l) %15c%v" });
	m_pattern.emplace(LogLevel::Warn, Tokenizer{ "%10c[%h:%m:%s:%T]%14c[Warn ]%8c(%12F l.%4l) %15c%v" });
	m_pattern.emplace(LogLevel::Error, Tokenizer{ "%10c[%h:%m:%s:%T]%12c[Error]%8c(%12F l.%4l) %15c%v" });
}

void ska::loggerdetail::Logger::consumeNow(LogEntry& self) {
	assert (m_pattern.find(self.getContext().logLevel) != m_pattern.end());
	auto& currentPattern = m_pattern.at(self.getContext().logLevel);
	consumeNowWithPattern(currentPattern, self);
	self.disable();
}

void ska::loggerdetail::Logger::consumeNowWithPattern(const Tokenizer& pattern, LogEntry& self, bool newLine) {
	for (auto& o : m_output) {
		if (o.isATarget(self)) {
			for (auto& token : pattern) {
				consumeNowOnOutput(token, o, self);
			}
			if(newLine) {
				o.endLine();
			}
		}
	}
}

void ska::loggerdetail::Logger::consumeNowOnOutput(const Token& token, LogTarget& o, LogEntry& self) {
	if(o.applyTokenOnOutput(self, token) == TokenConsumeType::ComplexPattern) {
		self.nextPatternReccursionLevel();
		consumeNowWithPattern(Tokenizer{self.getMessage()}, self, false);
	}
}

void ska::loggerdetail::Logger::enableComplexLogging() {
	m_enableComplexLogging = true;
	for(auto& target : m_output) {
		target.enableComplexLogging();
	}
}

void ska::loggerdetail::Logger::setPattern(LogLevel logLevel, std::string pattern) {
	auto existingLoglevel = m_pattern.find(logLevel);
	if(existingLoglevel != m_pattern.end()) {
		m_pattern.erase(existingLoglevel);
	}
	m_pattern.emplace(logLevel, Tokenizer {std::move(pattern)});
}

void ska::loggerdetail::Logger::addOutputTarget(std::ostream& output, LogFilter filter) {
	m_output.emplace_back(output, filter, m_enableComplexLogging, output.rdbuf() == std::cout.rdbuf());
}
