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

#define SKA_LIB_CALLS_DEFINE(STRUCT_NAME, ENUM, FUNCTION, NAME)\
template <>\
std::string STRUCT_NAME<ENUM>::name = NAME;\
template <>\
struct HasFunctionFormatTrait<STRUCT_NAME<ENUM>> {\
using FunctionFormat = FUNCTION;\
}

namespace ska {

	template <class T>
	struct HasFunctionFormatTrait;
	
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

		template <class Name>
		using Caller = function_caller<typename HasFunctionFormatTrait<Name>::FunctionFormat>;

		template <class Name>
		using EnabledReturn = std::enable_if_t<Caller<Name>::HasReturn>;

		template <class Name>
		using DisabledReturn = std::enable_if_t<!Caller<Name>::HasReturn>;

		template <class Name, class ... Args, class = EnabledReturn<Name>>
		auto call(Args&&... args) const {
			checkType<Name>();
			return Caller<Name>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
		}

		template <class Name, class ... Args, class = DisabledReturn<Name>>
		void call(Args&&... args) const {
			checkType<Name>();
			Caller<Name>::Caller::call(*m_cache.template get<Name>(), std::forward<Args>(args)...);
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
