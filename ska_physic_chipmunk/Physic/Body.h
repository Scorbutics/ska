#pragma once
#include <chipmunk.h>
#include <gsl/pointers>
#include "Utils/MovableNonCopyable.h"
#include "ECS/ECSDefines.h"

namespace ska {
	namespace cp {
		class Vect;
		
		enum class BodyType {
			DYNAMIC,
			KINEMATIC,
			STATIC
		};

		class Body :
			public MovableNonCopyable {
		public:
			Body();
			Body(Body&&) noexcept;
			Body& operator=(Body&&) noexcept;
			virtual ~Body();

			cpBody& body();
			const cpBody& body() const;

			void applyForce(const Vect& force);
			void applyImpulse(const Vect& impulse);
			
			void setEntity(EntityId entity);

			static Body fromMoment(double mass, double moment);
			static Body fromRadius(double mass, double radius);
			static Body fromKinematic();
			static Body fromMassAndSizeForBox(double mass, unsigned int w, unsigned int h);

			cpVect getPosition() const;
			cpVect getRotation() const;
			cpVect getVelocity() const;

			void linkConstraint(std::size_t constraintSpaceIndex);
			void linkShape(std::size_t shapeSpaceIndex);

			const std::vector<std::size_t>& getConstraints() const;
			const std::vector<std::size_t>& getShapes() const;

			void setPosition(const Vect& p);
			void setType(BodyType type);
			void setVelocity(const Vect& v);

		private:
			void loadFromMoment(double mass, double moment);
			void loadFromRadius(double mass, double radius);
			void loadBoxFromMassAndSize(double mass, unsigned int w, unsigned int h);
			void loadFromKinematic();

			void free();
			gsl::owner<cpBody*> m_body;

			EntityId m_entity;
			std::vector<std::size_t> m_constraints;
			std::vector<std::size_t> m_shapes;
		};
	}
}
