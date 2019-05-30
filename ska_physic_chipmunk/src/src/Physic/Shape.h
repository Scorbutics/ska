#pragma once
#include <chipmunk/chipmunk.h>
#include <gsl/pointers>
#include "Base/Values/MovableNonCopyable.h"
#include "Vect.h"

namespace ska {
	struct Rectangle;
}

namespace ska {
	namespace cp {
		class Body;

		class Shape : 
			public MovableNonCopyable {
		public:
			Shape();
			Shape(Shape&&) noexcept;
			Shape& operator=(Shape&& sh) noexcept;

			virtual ~Shape();
			
			const cpShape& shape() const;
			cpShape& shape();

			ska::Rectangle getDimensions() const;
			cpShapeFilter getFilter() const;
			cpCollisionType getCollisionType() const;

			static Shape fromSegment(cpBody& body, const Vect& a, const Vect& b, double radius);
			static Shape fromCircle(cpBody& body, double radius, const Vect& offset = Vect{});
			static Shape fromBox(cpBody& body, const ska::Rectangle& r, double radius = 0.);

			void setFriction(float friction);
			void setBounciness(float bouncinesss);
			void setFilter(cpGroup group, cpBitmask categories, cpBitmask mask);
			void setCollisionType(cpCollisionType collisionType);

		private:
			void loadFromSegment(cpBody& body, const Vect& a, const Vect& b, double radius);
			void loadFromBox(cpBody& body, const ska::Rectangle& r, double radius);
			void loadFromCircle(cpBody& body, double radius, const Vect& offset);

			void free();

			gsl::owner<cpShape*> m_shape;
		};
	}
}
