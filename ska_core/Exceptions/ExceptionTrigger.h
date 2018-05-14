#pragma once
#include <functional>
#include <stack>
#include "../Utils/SkaConstants.h"
#include "GenericException.h"
#include "../Logging/Logger.h"

namespace ska {

	struct ExceptionCatcher {
		ExceptionCatcher(std::stack<std::reference_wrapper<ExceptionCatcher>>& container, 
				std::function<void(const GenericException& message)> callback) :
				m_handler(std::move(callback)),
				m_container(container) {
		}
		
		ExceptionCatcher(const ExceptionCatcher&) = delete;
		ExceptionCatcher& operator=(const ExceptionCatcher&) = delete;
		
		void operator()(const GenericException& ge) {
			m_handler(ge);
		}

		~ExceptionCatcher() {
			if(!m_container.empty()) {
				m_container.pop();
			}
		}

	private:
		std::function<void(const GenericException& message)> m_handler;
		std::stack<std::reference_wrapper<ExceptionCatcher>>& m_container;
	};

	static const std::function<void(const GenericException& ge)> ExceptionAbort;
	
	class ExceptionTriggerer {
	public:
		~ExceptionTriggerer() = default;

		static ExceptionTriggerer& get() {
			static ExceptionTriggerer instance;
			return instance;
		}

		void trigger(const GenericException& ge) {
			if (!m_exceptions.empty()) {
				m_exceptions.top().get()(ge);
				decltype(m_exceptions)().swap(m_exceptions);
			}
		}

		ExceptionCatcher createCatcher(std::function<void(const GenericException& message)> catcher) {
			return ExceptionCatcher(m_exceptions, std::move(catcher));
		}

		void addCallback(ExceptionCatcher& catcher) {
			m_exceptions.push(catcher);
		}

	private:
		ExceptionTriggerer() = default;
		std::stack<std::reference_wrapper<ExceptionCatcher>> m_exceptions;
	};

	template<class Exception>
	void ExceptionTrigger(const std::string& message) {
		#ifdef SKA_EXCEPTIONS_DISABLED
		ExceptionTriggerer::get().trigger(Exception(message.c_str()));
		#else
		throw Exception(message.c_str());
		#endif
	}

	#ifdef SKA_EXCEPTIONS_DISABLED
		#define skaTryCatch(TryCode, ExcepType, ExcepName, CatchCode) \
		auto ExcepNameEnglober = ska::ExceptionTriggerer::get().createCatcher([&](const ska::GenericException& ExcepName) {\
			do { CatchCode } while (0);\
		});\
		ska::ExceptionTriggerer::get().addCallback(ExcepNameEnglober);\
		do { TryCode } while (0);
	#else
		#define skaTryCatch(TryCode, ExcepType, ExcepName, CatchCode) \
		try { do { TryCode } while (0); } catch (ExcepType& ExcepName) { do { CatchCode } while (0); }
	#endif

}
