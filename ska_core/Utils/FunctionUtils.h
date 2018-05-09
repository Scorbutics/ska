#pragma once
#include <tuple>
#include <type_traits>

namespace ska {
	template <typename Function> struct split_function_args;
	template <typename Ret, typename ... Args>
	struct split_function_args<Ret(Args...)> {
		using ArgumentsTuple = std::tuple<Args...>;
		using Return = Ret;
	};

	template <typename Function> struct function_caller;

	template <typename Ret, typename ... Args>
	struct function_caller<Ret(Args...)> {
	private:
		using FunctionType = Ret(Args...);
	public:
		using ReturnType = Ret;
		static constexpr auto HasReturn = !std::is_same<Ret, void>::value;

		template <bool>
		struct CallerE;

		template<>
		struct CallerE<false> {
			static void call(const void* functionPtr, Args&&... args) {
				auto typeSafeFunctionPtr = reinterpret_cast<Ret(*)(Args...)>(functionPtr);
				(*typeSafeFunctionPtr)(std::forward<Args>(args)...);
			}
		};

		template<>
		struct CallerE<true> {
			static ReturnType call(const void* functionPtr, Args&&... args) {
				auto typeSafeFunctionPtr = reinterpret_cast<Ret(*)(Args...)>(functionPtr);
				return (*typeSafeFunctionPtr)(std::forward<Args>(args)...);
			}
		};

		using Caller = CallerE<HasReturn>;
	};
}
