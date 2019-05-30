#pragma once
#include <chipmunk/chipmunk.h>
#include <gsl/pointers>
#include "Base/Values/MovableNonCopyable.h"
#include "Vect.h"

namespace ska {
	namespace cp {

		class Constraint :
			public MovableNonCopyable {
		public:
			Constraint();
			Constraint(Constraint&&) noexcept;
			Constraint& operator=(Constraint&&) noexcept;
			virtual ~Constraint();
			
			cpConstraint& constraint();
			const cpConstraint& constraint() const;

			static Constraint buildGearJoint(cpBody& body1, cpBody& body2, double phase, double ratio);
			static Constraint buildPivotJoint(cpBody& body1, cpBody& body2, Vect anchor1 = Vect{}, Vect anchor2 = Vect{});

			void setMaxForce(double maxForce);
			void setMaxBias(double maxBias);
			void setErrorBias(double errorBias);

		private:
			Constraint(cpBody& body1, cpBody& body2, Vect anchor1, Vect anchor2);
			Constraint(cpBody& body1, cpBody& body2, double phase, double ratio);
			void free();
			gsl::owner<cpConstraint*> m_joint;
		};
	}
}
