#pragma once

#include "Value/BytecodeInstructionOutput.h"

namespace ska {
	namespace bytecode {
		class Generator;
		class GenerationContext;

		class GeneratorOperatorUnit {
		public:
			virtual InstructionOutput generate(GenerationContext& node) = 0;
			virtual ~GeneratorOperatorUnit() = default;
		};

		class GeneratorOperatorBase :
			public GeneratorOperatorUnit {
		public:
			GeneratorOperatorBase(Generator& generator) :
				m_generator(generator) {}


			InstructionOutput generateNext(GenerationContext node);

			template <class Function, class ... Args>
			decltype(auto) applyGenerator(Function f, Args&& ... args) {
				return f(m_generator, std::forward<Args>(args)...);
			}

		private:
			Generator& m_generator;
		};
	}
}
