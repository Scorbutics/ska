#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/NonCopyable.h"

namespace ska {
	namespace cp {
		class Vect;

		class Shape : 
			public NonCopyable {
		public:
			Shape(Shape&&);
			Shape& operator=(Shape&& sh);

			virtual ~Shape();
			cpShape* shape() const;

			static Shape fromSegment(cpBody* body, const Vect& a, const Vect& b, float radius);
			static Shape fromCircle(cpBody *body, float radius, const Vect& offset);

			void setFriction(float friction);

		private:
			Shape();
			void loadFromSegment(cpBody *body, const Vect& a, const Vect& b, float radius);
			void loadFromCircle(cpBody *body, float radius, const Vect& offset);

			void free();
			gsl::owner<cpShape*> m_shape;
		};
	}
}
