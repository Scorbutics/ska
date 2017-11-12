#pragma once
#include "../../../Utils/RectangleUtils.h"
#include "../../../Logging/Logger.h"

namespace ska {
	class CollisionContact {
	public:
		CollisionContact() :
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

	private:
		Rectangle m_overlap;
		float m_penetration;
		Point<float> m_normal;

		CollisionContact(Rectangle overlap_, float penetration_, Point<float> normal_) :
			m_overlap(overlap_),
			m_penetration(penetration_), 
			m_normal(normal_) {
		}

		static CollisionContact build(Rectangle overlap, Point<int> pcA, Point<int> pcB) {
			float penetration;
			Point<float> normal;

			if (overlap.w < overlap.h) {
				const auto vectorAToBX = pcA.x - pcB.x;
				normal.x = vectorAToBX < 0 ? -1.F : 1.F;
				normal.y = 0;
				penetration = static_cast<float>(overlap.w);
				SKA_LOG_INFO((vectorAToBX < 0 ? "<" : ">"));
			} else {
				const auto vectorAToBY = pcA.y - pcB.y;
				normal.x = 0;
				normal.y = vectorAToBY < 0 ? -1.F : 1.F;
				penetration = static_cast<float>(overlap.h);
				SKA_LOG_INFO((vectorAToBY < 0 ? "^" : "v"));
			}
			return CollisionContact(overlap, penetration, normal);
		}
	};
}
