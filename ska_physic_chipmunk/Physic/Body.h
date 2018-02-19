#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/NonCopyable.h"

namespace ska {
	namespace cp {
		class Vect;

		class Body :
			public NonCopyable {
		public:
			Body(Body&&) = default;
			virtual ~Body();
			cpBody* body() const;

			static Body&& fromMoment(double mass, double moment);
			static Body&& fromRadius(double mass, double radius);

			cpVect getPosition() const;
			cpVect getVelocity() const;

			void setPosition(const Vect& p) const;

		private:
			Body();
			void loadFromMoment(double mass, double moment);
			void loadFromRadius(double mass, double radius);

			void free();
			gsl::owner<cpBody*> m_body;
		};
	}
}
