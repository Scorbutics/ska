#pragma once
#include <sstream>
#include <vector>
#include <string>

namespace ska {
	namespace loggerdetail {
        template <class ...T>
        struct MessagesBuilder;

        template <>
        struct MessagesBuilder<> {
            static void buildMessages(std::vector<std::string>& messagesOutput) {}
        };

        template <class T1, class ...T>
        struct MessagesBuilder<T1, T...> {
            static void buildMessages(std::vector<std::string>& messagesOutput, const T1& message, const T& ... messageList) {
                {
                    auto ss = std::stringstream {};
                    ss << message;
                    messagesOutput.push_back(ss.str());
                }
                MessagesBuilder<T...>::buildMessages(messagesOutput, std::forward<const T&>(messageList) ...);
            }
        };
	}
}