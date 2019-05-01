#pragma once
#include <tuple>
#include <type_traits>
#include <cassert>

namespace ska {
	template <typename Function> struct split_function_args;
	template <typename Ret, typename ... Args>
	struct split_function_args<Ret(Args...)> {
		using ArgumentsTuple = std::tuple<Args...>;
		using Return = Ret;
	};

	namespace detail {
		template <bool, class Ret, class ... Args>
		struct Caller;
		
		template<class Ret, class ... ArgsF>
		struct Caller<false, Ret, ArgsF...> {
			template <class ... Args>
			static void call(void* functionPtr, Args&&... args) {
				const auto typeSafeFunctionPtr = reinterpret_cast<Ret(*)(ArgsF...)>(functionPtr);
				(*typeSafeFunctionPtr)(std::forward<Args>(args)...);
			}
		};

		template<class Ret, class ... ArgsF>
		struct Caller<true, Ret, ArgsF...> {
			template <class ... Args>
			static Ret call(void* functionPtr, Args&&... args) {
				const auto typeSafeFunctionPtr = reinterpret_cast<Ret(*)(ArgsF...)>(functionPtr);
				return (*typeSafeFunctionPtr)(std::forward<Args>(args)...);
			}
		};
	}

	template <typename Function> struct function_caller;

	template <typename Ret, typename ... Args>
	struct function_caller<Ret(Args...) const> {
	private:
		using FunctionType = Ret(Args...);
	public:
		using ReturnType = Ret;
		static constexpr auto HasReturn = !std::is_same<Ret, void>::value;

		using Caller = detail::Caller<HasReturn, Ret, Args...>;
	};

	template <typename Ret, typename ... Args>
	struct function_caller<Ret(Args...)> {
	private:
		using FunctionType = Ret(Args...);
	public:
		using ReturnType = Ret;
		static constexpr auto HasReturn = !std::is_same<Ret, void>::value;

		using Caller = detail::Caller<HasReturn, Ret, Args...>;
	};

	
}
