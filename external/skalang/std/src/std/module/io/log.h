#pragma once
#include <iostream>
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOLogModule : public Module<Interpreter> {
        public:
            IOLogModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.io.log", "std:std.io.log"},
                m_builder(BridgeBuilder<Interpreter> { Module<Interpreter>::m_bridge }) {

                m_builder.bindField("print", [&](std::vector<ska::NodeValue> params) {
                    std::cout << params[0].convertString() << std::endl;
                    return NodeValue{};
                });

                m_builder.generate();
            }
            ~IOLogModule() override = default;
        private:
            BridgeBuilder<Interpreter> m_builder;
        };
    }
}