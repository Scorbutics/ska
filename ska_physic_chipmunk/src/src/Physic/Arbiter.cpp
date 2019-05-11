#include "Arbiter.h"
#include "Core/Utils/RectangleUtils.h"

ska::cp::Arbiter::Arbiter(cpArbiter& arb) :
	m_instance(&arb) {
}

cpContactPointSet ska::cp::Arbiter::getContactPoints() const {
	return cpArbiterGetContactPointSet(m_instance);
}

std::size_t ska::cp::Arbiter::getContactPointsNumber() const {
	return cpArbiterGetCount(m_instance);
}

std::pair<cpBody*, cpBody*> ska::cp::Arbiter::getBodies() const {
	cpBody* a;
	cpBody* b;
	cpArbiterGetBodies(m_instance, &a, &b);
	return std::make_pair(a, b);
}

cpVect ska::cp::Arbiter::getNormal() const {
	return cpArbiterGetNormal(m_instance);
}

std::pair<unsigned int, ska::Rectangle> ska::cp::Arbiter::calculateOverlap() const {
	auto overlaps = std::vector<ska::Rectangle>{};

	const auto cpSet = getContactPoints();
	if(cpSet.count == 0 || cpSet.count > 2) {
		return {};
	}

	for (auto i = 0; i < cpSet.count; i++) {
		overlaps.push_back(ska::RectangleUtils::createRectangleFromPoints(ska::Point<int> {
			static_cast<int>(cpSet.points[i].pointA.x),
				static_cast<int>(cpSet.points[i].pointA.y)
		}, ska::Point<int> {
			static_cast<int>(cpSet.points[i].pointB.x),
				static_cast<int>(cpSet.points[i].pointB.y)
		}));
	}

	auto finalOverlap = overlaps[0];
	if (overlaps.size() >= 2) {
		finalOverlap.w = finalOverlap.w == 0 ? ska::NumberUtils::absolute(overlaps[0].x - overlaps[1].x) : finalOverlap.w;
		finalOverlap.h = finalOverlap.h == 0 ? ska::NumberUtils::absolute(overlaps[0].y - overlaps[1].y) : finalOverlap.h;
		finalOverlap.w = finalOverlap.w == 0 ? 1 : finalOverlap.w;
		finalOverlap.h = finalOverlap.h == 0 ? 1 : finalOverlap.h;
	} else {
		finalOverlap.w = finalOverlap.w == 0 ? 1 : finalOverlap.w;
		finalOverlap.h = finalOverlap.h == 0 ? 1 : finalOverlap.h;
	}

	const auto penetrationX = (cpSet.normal.x * finalOverlap.w);
	const auto penetrationY = (cpSet.normal.y * finalOverlap.h);
	return std::make_pair(penetrationX * penetrationX + penetrationY * penetrationY, finalOverlap);
}
