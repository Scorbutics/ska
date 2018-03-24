#pragma once

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
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::POST> {
			using type = cpCollisionPostSolveFunc;
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::BEGIN> {
			using type = cpCollisionBeginFunc;
		};

		template <>
		struct CollisionHandlerTypeFunc<CollisionHandlerType::SEPARATE> {
			using type = cpCollisionSeparateFunc;
		};

		class CollisionHandlerData {
		public:
			CollisionHandlerData() = default;
			~CollisionHandlerData() = default;

			template <CollisionHandlerType type>
			void setHandler(typename cp::CollisionHandlerTypeFunc<type>::type func);

			template <CollisionHandlerType type>
			typename cp::CollisionHandlerTypeFunc<type>::type getHandler();

			CollisionHandlerType getType() const {
				return m_type;
			}

		private:
			cpCollisionBeginFunc m_begin;
			cpCollisionPreSolveFunc m_preSolve;
			cpCollisionPostSolveFunc m_postSolve;
			cpCollisionSeparateFunc m_separate;

			CollisionHandlerType m_type;
		};
	}

	template <>
	inline void cp::CollisionHandlerData::setHandler<cp::CollisionHandlerType::BEGIN>(cpCollisionBeginFunc func){
		m_begin = func;
		m_type = cp::CollisionHandlerType::BEGIN;
	}

	template <>
	inline void cp::CollisionHandlerData::setHandler<cp::CollisionHandlerType::PRE>(cpCollisionPreSolveFunc func){
		m_preSolve = func;
		m_type = cp::CollisionHandlerType::PRE;
	}

	template <>
	inline void cp::CollisionHandlerData::setHandler<cp::CollisionHandlerType::POST>(cpCollisionPostSolveFunc func){
		m_postSolve = func;
		m_type = cp::CollisionHandlerType::POST;
	}

	template <>
	inline void cp::CollisionHandlerData::setHandler<cp::CollisionHandlerType::SEPARATE>(cpCollisionSeparateFunc func){
		m_separate = func;
		m_type = cp::CollisionHandlerType::SEPARATE;
	}

	template <>
	inline cp::CollisionHandlerTypeFunc<cp::CollisionHandlerType::BEGIN>::type cp::CollisionHandlerData::getHandler<cp::CollisionHandlerType::BEGIN>(){
		return m_begin;
	}

	template <>
	inline cp::CollisionHandlerTypeFunc<cp::CollisionHandlerType::PRE>::type cp::CollisionHandlerData::getHandler<cp::CollisionHandlerType::PRE>(){
		return m_preSolve;
	}

	template <>
	inline cp::CollisionHandlerTypeFunc<cp::CollisionHandlerType::POST>::type cp::CollisionHandlerData::getHandler<cp::CollisionHandlerType::POST>(){
		return m_postSolve;
	}
	
	template <>
	inline cp::CollisionHandlerTypeFunc<cp::CollisionHandlerType::SEPARATE>::type cp::CollisionHandlerData::getHandler<cp::CollisionHandlerType::SEPARATE>(){
		return m_separate;
	}
}