#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/NonCopyable.h"

namespace ska {
	namespace cp {
		class Vect;

		class Constraint :
			public NonCopyable {
		public:
			Constraint();
			Constraint(Constraint&&) noexcept;
			Constraint& operator=(Constraint&&) noexcept;
			virtual ~Constraint();
			cpConstraint* constraint();
			
			static Constraint buildGearJoint(cpBody& body1, cpBody& body2, double phase, double ratio);
			static Constraint buildPivotJoint(cpBody& body1, cpBody& body2, const Vect& anchor1, const Vect& anchor2);

			void setMaxForce(double maxForce);
			void setMaxBias(double maxBias);

		private:
			Constraint(cpBody& body1, cpBody& body2, const Vect& anchor1, const Vect& anchor2);
			Constraint(cpBody& body1, cpBody& body2, double phase, double ratio);
			void free();
			gsl::owner<cpConstraint*> m_joint;
		};
	}
}
