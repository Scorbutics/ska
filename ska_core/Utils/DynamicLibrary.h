#pragma once
#include <string>
#include "SkaConstants.h"
#include "CharCache.h"
#include "FunctionUtils.h"
#include "ContainsTypeTuple.h"
#include "TupleUtils.h"

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

	protected:
		DynamicLibrary(std::string libraryPath) :
			m_libraryPath(std::move(libraryPath)),
			m_instance(m_libraryPath) {
			int _[] = { 0, (buildCache<FunctionName>(m_instance) , 0)... };
			(void)_;
		}
		
		template <class Name, class Function, class ... Args, class = std::enable_if_t<function_caller<Function>::HasReturn>>
		auto call(Args&&... args) {
			static_assert(meta::contains<Name, FunctionName...>::value, "This function name is undefined for this dynamic library");			
			return function_caller<Function>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
		}

		template <class Name, class Function, class ... Args, class = std::enable_if_t<!function_caller<Function>::HasReturn>>
		void call(Args&&... args) {
			static_assert(meta::contains<Name, FunctionName...>::value, "This function name is undefined for this dynamic library");
			function_caller<Function>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
		}

	private:
		template<class Func>
		void buildCache(const DynamicLibraryInstance& impl) {
			const auto& funcName = Func::name;
			auto [function, error] = impl.getFunction(funcName.c_str());
			if (function != nullptr) {
				m_cache.set(Func::id, std::move(function));
			} else {
				throw InputException("Unable to find the function " + funcName + " in the module " + m_libraryPath + ". Error logged : " + error);
			}
			
		}

		dynlib::Cache<void*, FunctionName...> m_cache;
		std::string m_libraryPath;
		DynamicLibraryInstance m_instance;
	};
}
