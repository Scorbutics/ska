#pragma once
#include <functional>
#include <gsl/pointers>
#include <vector>
#include "Base/Values/MovableNonCopyable.h"
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

			Space* space {};
			SpaceUserData() = default;

			std::tuple<
				//BEGIN
				SpaceCollisionCallbackContainer<bool>,
				//PRE
				SpaceCollisionCallbackContainer<bool>,
				//POST
				SpaceCollisionCallbackContainer<void>,
				//SEPARATE
				SpaceCollisionCallbackContainer<void>> defaultCallbacks;
			
		};

		std::pair<ska::EntityId*, ska::cp::SpaceUserData&> SpaceCollisionCallbackExtractUserData(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr);

		class Space :
			public MovableNonCopyable {

			template <CollisionHandlerType type>
			using CollisionHandlerCallbackFromType = SpaceUserData::SpaceCollisionCallback<typename CollisionHandlerTypeFunc<type>::returnType>;

		public:
			void initCollisionHandlers();
			Space();
			Space(Space&&) noexcept;
			Space& operator=(Space&&) noexcept;
			virtual ~Space();

			cpBody& getStaticBody();
			// True when a shape of the static body (the walls of the map) overlaps the box
			bool staticShapeIn(const cpBB& box) const;
			// True when any shape (a wall, a character...) overlaps the box
			bool shapeIn(const cpBB& box) const;
			// Bounding box of a shape
			cpBB shapeBox(std::size_t shapeIndex) const;
			
			void setGravity(const Vect& v);

			std::size_t addConstraint(Body* linked, Body& linked2, Constraint c);
			std::size_t addShape(Body* linked, Shape shape);
			std::size_t addBody(Body body);
			
			void clear();

			Body& getBody(std::size_t index);
			Shape& getShape(std::size_t index);
			Constraint& getConstaint(std::size_t index);

			// A shape, a body (with its shapes and its constraints) taken out of the space : the indices of the others stay,
			// the next ones added take the places left
			void removeShape(std::size_t index);
			void removeBody(std::size_t index);
			// The bodies from firstIndex to lastIndex (excluded ; 0 : all after it)
			void eraseBodies(std::size_t firstIndex, std::size_t lastIndex = 0);

			void setIterations(int iterations);
			void setSleepTimeThreshold(float threshold);

			void step(double timestep);

			template <CollisionHandlerType type>
			std::size_t addDefaultCollisionCallback(CollisionHandlerCallbackFromType<type> callback) {
				auto& container = std::get<static_cast<int>(type)>(m_userData.defaultCallbacks);
				auto id = container.size();
				container.emplace(id, std::forward<CollisionHandlerCallbackFromType<type>>(callback));
				return id;
			}

			template <CollisionHandlerType type>
			bool removeDefaultCollisionCallback(std::size_t index) {
				auto& container = std::get<static_cast<int>(type)>(m_userData.defaultCallbacks);
				return container.erase(index) > 0;
			}

			void addCollisionHandler(unsigned int collisionTypeA, unsigned int collisionTypeB, CollisionHandlerData collisionHandlerData);
			void addWildcardCollisionHandler(unsigned int collisionType, CollisionHandlerData collisionHandlerData);

		private:
			void removeConstraint(std::size_t index);

			void addDefaultCollisionHandler(CollisionHandlerData collisionHandlerData);

			void load();
			void free();

			SpaceUserData m_userData;
			std::vector<Shape> m_shapes;
			std::vector<Body> m_bodies;
			std::vector<Constraint> m_constraints;
			// The places left by the removed ones
			std::vector<std::size_t> m_freeShapes;
			std::vector<std::size_t> m_freeBodies;
			std::vector<std::size_t> m_freeConstraints;
			gsl::owner<cpSpace*> m_space;
		};
	}
}
