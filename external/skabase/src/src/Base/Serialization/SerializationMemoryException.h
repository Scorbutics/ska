#pragma once
#include <vector>
#include <stdexcept>
#include "SerializerErrorData.h"

namespace ska {

	template <class T>
	struct IsCharLiteral : std::false_type {};

	template <std::size_t Size>
	struct IsCharLiteral<const char (&)[Size]> : std::true_type {
		static constexpr auto size = Size;
	};

	struct SerializationMemoryException : 
		public std::exception {
		SerializationMemoryException(std::vector<SerializerErrorData> data) : data(std::move(data)) {}
		std::vector<SerializerErrorData> data;

		char const* what() const noexcept override;

		template <class ... Args>
		void queue(char buffer[], std::size_t counter, Args&& ... args) const {
			(queue_unique<Args>(buffer, counter, args), ...);
		}

	private:
		template <class Arg>
		void queue_unique(char buffer[], std::size_t counter, Arg&& arg) const {
			if constexpr (IsCharLiteral<Arg>::value) {
				this->template queue<IsCharLiteral<Arg>::size>(buffer, counter, std::forward<Arg>(arg));
			} else {
				if constexpr (std::is_same_v<Arg, const char*>) {
					queue(buffer, counter, std::forward<Arg>(arg));
				} else {
					using ArgDecay = std::remove_const_t<std::remove_reference_t<Arg>>;
					if constexpr (std::is_same_v<ArgDecay, std::string>) {
						const auto size = arg.size();
						queue(buffer, counter, std::forward<Arg>(arg).c_str(), size);
					} else {
						auto str = std::to_string(std::forward<Arg>(arg));
						queue(buffer, counter, str.c_str(), str.size());
					}
				}
			}
		}

		template <std::size_t size>
		void queue(char buffer[], std::size_t counter, const char* str) const {
			auto oldCounter = counter;
			counter += size;
			queue_(buffer, counter, oldCounter, str);
		}

		void queue(char buffer[], std::size_t counter, const char* str, std::size_t size) const;
		void queue_(char buffer[], std::size_t counter, std::size_t oldCounter, const char* str) const;

		static constexpr auto MAX_BUF = 2048;

	};
}
