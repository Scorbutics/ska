#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/NonCopyable.h"

namespace ska {
	namespace cp {
		class Vect;
		
		enum class BodyType {
			DYNAMIC,
			KINEMATIC,
			STATIC
		};

		class Body :
			public NonCopyable {
		public:
			Body();
			Body(Body&&) noexcept;
			Body& operator=(Body&&) noexcept;
			virtual ~Body();
			cpBody* body() const;
			cpVect getRotation() const;
			void applyForce(const Vect& force);
			void applyImpulse(const Vect& impulse);

			static Body fromMoment(double mass, double moment);
			static Body fromRadius(double mass, double radius);

			cpVect getPosition() const;
			cpVect getVelocity() const;

			void setPosition(const Vect& p);
			void setType(BodyType type);
			void setVelocity(const Vect& v);

		private:
			void loadFromMoment(double mass, double moment);
			void loadFromRadius(double mass, double radius);

			void free();
			gsl::owner<cpBody*> m_body;
		};
	}
}
