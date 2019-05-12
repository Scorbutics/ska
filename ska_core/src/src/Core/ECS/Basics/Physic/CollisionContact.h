#pragma once
#include "../../../Utils/RectangleUtils.h"

namespace ska {
	class CollisionContact {
	public:
		CollisionContact() :
			m_noCollision(true),
			m_overlap({ 0, 0, 0, 0 }),
			m_penetration(0.F) {
		}

		CollisionContact(Rectangle boxEntityA, Rectangle boxEntityB) :
			CollisionContact(build(RectangleUtils::intersect(boxEntityA, boxEntityB), boxEntityA, boxEntityB)) {
		}

		CollisionContact(Rectangle overlap, Point<int> boxEntityA, Point<int> boxEntityB) :
			CollisionContact(build(overlap, boxEntityA, boxEntityB)) {
		}

		const Rectangle& overlap() const{
			return m_overlap;
		}

		float penetration() const {
			return m_penetration;
		}

		const Point<float>& normal() const {
			return m_normal;
		}

		bool hasCollision() const{
			return !m_noCollision;
		}

		CollisionContact(Rectangle overlap_, float penetration_, Point<float> normal_, bool collision) :
			m_normal(normal_),
			m_noCollision(!collision),
			m_overlap(overlap_),
			m_penetration(penetration_) {
		}

	private:
		Point<float> m_normal;
		bool m_noCollision;
		Rectangle m_overlap;
		float m_penetration;

		static CollisionContact build(Rectangle overlap, Point<int> pcA, Point<int> pcB) {
			auto penetration = 0.F;
			Point<float> normal;
			auto collision = false;

			if (overlap.w != 0 && overlap.h != 0) {
				collision = true;

				if (overlap.w < overlap.h) {
					const auto vectorAToBX = pcA.x - pcB.x;
					normal.x = vectorAToBX < 0 ? -1.F : 1.F;
					normal.y = 0;
					penetration = static_cast<float>(overlap.w);
					SLOG_STATIC(LogLevel::Info, CollisionContact) << (vectorAToBX < 0 ? "<" : ">");
				} else if (overlap.w > overlap.h) {
					const auto vectorAToBY = pcA.y - pcB.y;
					normal.x = 0;
					normal.y = vectorAToBY < 0 ? -1.F : 1.F;
					penetration = static_cast<float>(overlap.h);
					SLOG_STATIC(LogLevel::Info, CollisionContact)<< (vectorAToBY < 0 ? "^" : "v");
				} else {
					const auto vectorAToBX = pcA.x - pcB.x;
					const auto vectorAToBY = pcA.y - pcB.y;
					normal.x = vectorAToBX < 0 ? -1.F : 1.F;
					normal.y = vectorAToBY < 0 ? -1.F : 1.F;
					penetration = static_cast<float>(overlap.w);

					SLOG_STATIC(LogLevel::Info, CollisionContact) << (vectorAToBY < 0 ? "^" : "v", vectorAToBX < 0 ? "<" : ">");
				}
			}
			return CollisionContact(overlap, penetration, normal, collision);
		}
	};
}
