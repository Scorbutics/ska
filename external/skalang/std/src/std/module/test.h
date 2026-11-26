#pragma once
#include <stdexcept>
#include <string>
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace ska {
    namespace lang {
        // Assertions of the test scripts : Test = import "bind:std.native.test" (see the "std.test" script template).
        // A failed assertion throws : the script stops with the error.
        template <class Interpreter>
        class TestModule : public Module<Interpreter> {
        public:
            TestModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.test", "std:std.test"},
                m_builder(BridgeBuilder<Interpreter> { Module<Interpreter>::m_bridge }) {

                m_builder.bindField("check", [](std::vector<NodeValue> params) {
                    if (!params[0].nodeval<bool>()) {
                        throw std::runtime_error("check failed : " + *params[1].nodeval<StringShared>());
                    }
                    return NodeValue{};
                });
                m_builder.bindField("equal", [](std::vector<NodeValue> params) {
                    const auto& actual = *params[0].nodeval<StringShared>();
                    const auto& expected = *params[1].nodeval<StringShared>();
                    if (actual != expected) {
                        throw std::runtime_error("equal failed : " + *params[2].nodeval<StringShared>() + " (\"" + actual + "\" instead of \"" + expected + "\")");
                    }
                    return NodeValue{};
                });
                m_builder.bindField("fail", [](std::vector<NodeValue> params) {
                    throw std::runtime_error("failed : " + *params[0].nodeval<StringShared>());
                    return NodeValue{};
                });
                m_builder.generate();
            }
            ~TestModule() override = default;

        private:
            BridgeBuilder<Interpreter> m_builder;
        };
    }
}
