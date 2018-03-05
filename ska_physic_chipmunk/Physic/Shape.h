#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/NonCopyable.h"

namespace ska {
	struct Rectangle;
}

namespace ska {
	namespace cp {
		class Body;
		class Vect;

		class Shape : 
			public NonCopyable {
		public:
			Shape();
			Shape(Shape&&) noexcept;
			Shape& operator=(Shape&& sh) noexcept;

			virtual ~Shape();
			cpShape* shape() const;

			static Shape fromSegment(cpBody* body, const Vect& a, const Vect& b, double radius);
			static Shape fromCircle(cpBody* body, double radius, const Vect& offset);
			static Shape fromBox(cpBody* body, const ska::Rectangle& r, double radius);

			void setFriction(float friction);

		private:
			void loadFromSegment(cpBody *body, const Vect& a, const Vect& b, double radius);
			void loadFromBox(cpBody* body, const ska::Rectangle& r, double radius);
			void loadFromCircle(cpBody *body, double radius, const Vect& offset);

			void free();
			gsl::owner<cpShape*> m_shape;
		};
	}
}
