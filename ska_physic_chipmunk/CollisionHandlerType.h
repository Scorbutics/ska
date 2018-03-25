#pragma once
#include <cassert>
#include <variant>

namespace ska {
	namespace cp {
		enum class CollisionHandlerType {
			BEGIN,
			PRE,
			POST,
			SEPARATE
		};

		template <CollisionHandlerType type>
		struct CollisionHandlerTypeFunc;

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::PRE> {
			using type = cpCollisionPreSolveFunc;
			type func;
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::POST> {
			using type = cpCollisionPostSolveFunc;
			type func;
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::BEGIN> {
			using type = cpCollisionBeginFunc;
			type func;
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::SEPARATE> {
			using type = cpCollisionSeparateFunc;
			type func;
		};

		
		class CollisionHandlerData {
		public:
			template <CollisionHandlerType type>
			CollisionHandlerData(cp::CollisionHandlerTypeFunc<type> func) : 
				m_handle(func),
				m_type(type) {
			}
			~CollisionHandlerData() = default;

			void setupCollisionHandler(cpCollisionHandler& collisionHandler) const {
				switch(m_type) {
				case CollisionHandlerType::PRE: collisionHandler.preSolveFunc = std::get<static_cast<int>(CollisionHandlerType::PRE)>(m_handle).func; break;
				case CollisionHandlerType::BEGIN: collisionHandler.beginFunc = std::get<static_cast<int>(CollisionHandlerType::BEGIN)>(m_handle).func; break;
				case CollisionHandlerType::POST: collisionHandler.postSolveFunc = std::get<static_cast<int>(CollisionHandlerType::POST)>(m_handle).func; break;
				case CollisionHandlerType::SEPARATE: collisionHandler.separateFunc = std::get<static_cast<int>(CollisionHandlerType::SEPARATE)>(m_handle).func; break;
				default: assert(false, "Bad initialization");
				}
			}

		private:			
			const std::variant<
				CollisionHandlerTypeFunc<CollisionHandlerType::BEGIN>, 
				CollisionHandlerTypeFunc<CollisionHandlerType::PRE>, 
				CollisionHandlerTypeFunc<CollisionHandlerType::POST>, 
				CollisionHandlerTypeFunc<CollisionHandlerType::SEPARATE>> m_handle;

			const CollisionHandlerType m_type;
		};
	}
}
