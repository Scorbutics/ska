#pragma once
#include <functional>
#include <gsl/pointers>
#include <vector>
#include "Utils/MovableNonCopyable.h"
#include "Shape.h"
#include "Body.h"
#include "Constraint.h"
#include "../CollisionHandlerType.h"

namespace ska {
	namespace cp {
		class CollisionHandlerData;
		enum class CollisionHandlerType;
		class Vect;
		class Space;

		struct SpaceUserData {
			template <class ReturnType>
			using SpaceCollisionCallback = std::function<ReturnType(cpArbiter&, Space&, EntityId&)>;

			Space& space;
			SpaceUserData(Space& space) : space(space) {};

			std::tuple<
				//BEGIN
				std::vector<SpaceCollisionCallback<bool>>,
				//PRE
				std::vector<SpaceCollisionCallback<bool>>,
				//POST
				std::vector<SpaceCollisionCallback<void>>,
				//SEPARATE
				std::vector<SpaceCollisionCallback<void>>> callbacks;
			
		};

		class Space :
			public MovableNonCopyable {

		public:
			void initCollisionHandlers();
			Space();
			Space(Space&&) noexcept;
			virtual ~Space();

			cpBody* getStaticBody();
			
			void setGravity(const Vect& v);

			Constraint&  addConstraint(Constraint c);
			Shape& addShape(Shape shape);
			Body& addBody(Body body);
			std::vector<Body>& getBodies();
			std::vector<Shape>& getShapes();

			void step(double timestep);

			template <CollisionHandlerType type>
			void addDefaultCollisionCallback(SpaceUserData::SpaceCollisionCallback<typename CollisionHandlerTypeFunc<type>::returnType> callback) {
				std::get<static_cast<int>(type)>(m_userData.callbacks).push_back(std::forward<SpaceUserData::SpaceCollisionCallback<typename CollisionHandlerTypeFunc<type>::returnType>>(callback));
			}

		private:
			void addCollisionHandler(unsigned int collisionTypeA, unsigned int collisionTypeB, CollisionHandlerData collisionHandlerData);
			void addDefaultCollisionHandler(CollisionHandlerData collisionHandlerData);

			void load();
			void free();

			SpaceUserData m_userData;
			std::vector<Shape> m_shapes;
			std::vector<Body> m_bodies;
			std::vector<Constraint> m_constraints;
			gsl::owner<cpSpace*> m_space;
		};
	}
}
