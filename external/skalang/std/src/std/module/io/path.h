#pragma once
#include "std/module.h"
#include "Base/IO/Files/FileUtils.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOPathModule : public Module<Interpreter> {
        public:
            IOPathModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> { config, "std.native.io.path", "std:std.io.path" },
                m_constructor(BridgeBuilder<Interpreter> { Module<Interpreter>::m_bridge, "Fcty" }) {

                m_constructor.bindField("canonical", [&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
                    return std::make_shared<std::string>(FileUtils::getCanonicalPath(*m_constructor.template param<StringShared>(0)));
                });

                m_constructor.generate();
            }
            ~IOPathModule() override = default;
        private:
            BridgeBuilder<Interpreter> m_constructor;
        };
    }
}
