#pragma once

#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOFileModule : public Module<Interpreter> {
        public:
            IOFileModule(ModuleConfiguration<Interpreter>& config)  :
                Module<Interpreter> {config, "std.native.io.file"}  {
				// TODO
            }

            ~IOFileModule() override = default;
        private:
        };
    }
}