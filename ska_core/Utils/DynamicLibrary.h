#pragma once
#include <string>
#include "SkaConstants.h"
#include "CharCache.h"
#include "FunctionUtils.h"
#include "ContainsTypeTuple.h"
#include "../Logging/Logger.h"

#if defined(SKA_PLATFORM_WIN)
#include "OSSpecific/DynamicLibraryWindows.h"
namespace ska {
	using DynamicLibraryInstance = DynamicLibraryWindows;
}
#elif defined(SKA_PLATFORM_LINUX)
#include "OSSpecific/DynamicLibraryUnix.h"
namespace ska {
	using DynamicLibraryInstance = DynamicLibraryUnix;
}
#endif

namespace ska {

	template<class ... FunctionName>
	class DynamicLibrary {
	public:
		virtual ~DynamicLibrary() = default;
		
		bool isLoaded() const {
			return m_instance.isLoaded();
		}

		template<class Name>
		bool hasLoaded() const {
			this->template checkType<Name>();
			return m_cache.template get<Name>() != nullptr;
		}

		template <class Name>
		bool loadToCache() {
			this->template checkType<Name>();
			if(this->template hasLoaded<Name>()) {
				return true;
			}
			this->template buildCache<Name>(m_instance);
			return this->template hasLoaded<Name>();
		}

	protected:
		DynamicLibrary(std::string libraryPath) :
			m_libraryPath(std::move(libraryPath)),
			m_instance(m_libraryPath) {
			if(isLoaded()) {
				int _[] = { 0, (buildCache<FunctionName>(m_instance) , 0)... };
				(void)_;
			}
		}

		template <class Name, class Function, class ... Args, class = std::enable_if_t<function_caller<Function>::HasReturn>>
		auto call(Args&&... args) {
			checkType<Name>();
			return function_caller<Function>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
		}

		template <class Name, class Function, class ... Args, class = std::enable_if_t<!function_caller<Function>::HasReturn>>
		void call(Args&&... args) {
			checkType<Name>();
			function_caller<Function>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
		}

	private:
		template <class Name>
		static constexpr void checkType() {
			static_assert(meta::contains<Name, FunctionName...>::value, "This function name is undefined for this dynamic library");			
		}

		template<class Func>
		void buildCache(const DynamicLibraryInstance& impl) {
			const auto& funcName = Func::name;
			auto [function, error] = impl.getFunction(funcName.c_str());
			if (function == nullptr) {
				SKA_LOG_MESSAGE("Unable to find the function " + funcName + " in the module " + m_libraryPath + ". Error logged : " + error);
			}
			m_cache.set(Func::id, std::move(function));
		}

		dynlib::Cache<void*, FunctionName...> m_cache;
		std::string m_libraryPath;
		DynamicLibraryInstance m_instance;
	};
}
