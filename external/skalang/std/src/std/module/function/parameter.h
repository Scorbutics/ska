#pragma once
#include <vector>

#include "Runtime/Value/NodeValue.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Runtime/Service/BridgeBuilder.h"

#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class ParameterModule : public Module<Interpreter> {
        public:
            ParameterModule(ModuleConfiguration<Interpreter>& config, const NodeValueArray& parameterValues) :
                Module<Interpreter> { config, "std.native.function.parameter", "std:std.function.parameter" },
                m_constructor(BridgeBuilder<Interpreter> { Module<Interpreter>::m_bridge }),
				m_parameters(parameterValues) {

                m_constructor.bindField("Get", [&](std::vector<ska::NodeValue>) -> ska::NodeValue {
                    return m_parameters;
                });
                m_constructor.generate();
            }

            ~ParameterModule() override = default;
        private:
            const NodeValueArray& m_parameters;
            BridgeBuilder<Interpreter> m_constructor;
        };
    }
}