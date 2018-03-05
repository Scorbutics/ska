#pragma once
#include <gsl/pointers>
#include <vector>
#include "Utils/NonCopyable.h"
#include "Shape.h"
#include "Body.h"

namespace ska {
	namespace cp {
		class Vect;

		class Space :
			public NonCopyable {
		public:
			Space();
			Space(Space&&) noexcept;
			Space& operator=(Space&&) noexcept;
			virtual ~Space();

			cpBody* getStaticBody();
			
			void setGravity(const Vect& v);
			Shape& addShape(Shape shape);
			Body& addBody(Body body);
			std::vector<ska::cp::Body>& getBodies();

			void step(double timestep);

		private:
			void load();
			void free();

			std::vector<Shape> m_shapes;
			std::vector<Body> m_bodies;
			gsl::owner<cpSpace*> m_space;
		};
	}
}
