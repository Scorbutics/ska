#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Rectangle.h"

namespace ska {
	namespace cp {
		class Arbiter {
		public:
			Arbiter(cpArbiter& arb);
			~Arbiter() = default;

			cpContactPointSet getContactPoints() const;
			std::size_t getContactPointsNumber() const;
			std::pair<cpBody*, cpBody*> getBodies() const;
			std::pair<unsigned int, Rectangle> calculateOverlap() const;

		private:
			gsl::not_null<cpArbiter*> m_instance;
		};
	}
}
