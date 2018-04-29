#pragma once
#include <functional>
#include <gsl/pointers>
#include <vector>
#include "Utils/MovableNonCopyable.h"
#include "Shape.h"
#include "Body.h"
#include "Constraint.h"
#include "../CollisionHandlerType.h"
#include "Arbiter.h"

namespace ska {
	namespace cp {
		class CollisionHandlerData;
		enum class CollisionHandlerType;
		class Vect;
		class Space;

		struct SpaceUserData {
			template <class ReturnType>
			using SpaceCollisionCallback = std::function<ReturnType(Arbiter&, Space&, EntityId*)>;

			template <class ReturnType>
			using SpaceCollisionCallbackContainer = std::unordered_map<std::size_t, SpaceCollisionCallback<ReturnType>>;

			Space& space;
			SpaceUserData(Space& space) : space(space) {};

			std::tuple<
				//BEGIN
				SpaceCollisionCallbackContainer<bool>,
				//PRE
				SpaceCollisionCallbackContainer<bool>,
				//POST
				SpaceCollisionCallbackContainer<void>,
				//SEPARATE
				SpaceCollisionCallbackContainer<void>> callbacks;
			
		};

		class Space :
			public MovableNonCopyable {

			template <CollisionHandlerType type>
			using CollisionHandlerCallbackFromType = SpaceUserData::SpaceCollisionCallback<typename CollisionHandlerTypeFunc<type>::returnType>;

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
			std::size_t addDefaultCollisionCallback(CollisionHandlerCallbackFromType<type> callback) {
				auto& container = std::get<static_cast<int>(type)>(m_userData.callbacks);
				auto id = container.size();
				container.emplace(id, std::forward<CollisionHandlerCallbackFromType<type>>(callback));
				return id;
			}

			template <CollisionHandlerType type>
			bool removeDefaultCollisionCallback(std::size_t index) {
				auto& container = std::get<static_cast<int>(type)>(m_userData.callbacks);
				return container.erase(index) > 0;
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
