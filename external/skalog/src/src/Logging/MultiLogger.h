#pragma once
#include <utility>
#include "LogContext.h"
#include "Logger.h"

namespace ska {
	template <class ... LoggerC>
	class MultiLogger {
		template <std::size_t... Idx>
		static auto make_index_dispatcher(std::index_sequence<Idx...>) {
			return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };
		}

		template <std::size_t N>
		static auto make_index_dispatcher() {
			return make_index_dispatcher(std::make_index_sequence<N>{});
		}

		template <typename Tuple, typename Func>
		static void for_each(Tuple&& t, Func&& f) {
			constexpr auto n = std::tuple_size<std::decay_t<Tuple>>::value;
			auto dispatcher = make_index_dispatcher<n>();
			dispatcher([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });
		}

	public:
		template <LogLevel logLevel, class Wrapped, unsigned long line>
		auto log(const char* functionName, const char* filename) {
			static_assert(logLevel != LogLevel::Disabled, "Unable to log at log level \"Disabled\"");
			if constexpr (logLevel >= LoggerClassLevel<Wrapped>::level) {
				return LogEntry{ 
					[&] (LogEntry& entry) {
						auto copied = entry;
						copied.disable();
						for_each(m_loggers, [&](auto& logger) {
							using LoggerType = typename std::remove_reference<decltype(logger)>::type;
							constexpr auto should = LoggerType::template accept<logLevel>();
							if constexpr (should) {
								logger.onDestroyEntry(copied);
							}
						});
					},
					loggerdetail::LogContext { logLevel, LoggerClassFormatter<Wrapped>::className, functionName, filename, line }
				};
			} else {
				(void) functionName;
				(void) filename;
				return loggerdetail::EmptyProxy{};
			}
		}

		void terminate() {
			for_each(m_loggers, [&](auto& logger) {
				logger.terminate();
			});
		}
		
		//Direct access for settings
		template <std::size_t index>
		auto& get() {
			return std::get<index>(m_loggers);
		}

	private:
		std::tuple<LoggerC...> m_loggers;
	};
}
