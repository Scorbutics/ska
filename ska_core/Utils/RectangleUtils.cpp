#include <algorithm>
#include "RectangleUtils.h"
#include "NumberUtils.h"

ska::RectangleUtils::RectangleUtils() {}

bool ska::RectangleUtils::collisionBoxABoxB(const Rectangle& rectA, const Rectangle& rectB) {

	if (rectA.x > rectB.x + rectB.w || rectA.x + rectA.w < rectB.x) {
		return false;
	}

	if (rectA.y > rectB.y + rectB.h || rectA.y + rectA.h < rectB.y) {
		return false;
	}

	return true;

}

ska::Rectangle ska::RectangleUtils::intersect(const Rectangle& r1, const Rectangle& r2) {
	Rectangle output;
	output.x = std::max(r1.x, r2.x);
	output.y = std::max(r1.y, r2.y);
	const auto xmax = std::min(r1.x + r1.w, r2.x + r2.w);
	const auto ymax = std::min(r1.y + r1.h, r2.y + r2.h);
	if (xmax >= output.x && ymax >= output.y) {
		output.w = xmax - output.x;
		output.h = ymax - output.y;
	} else {
		output.x = 0;
		output.y = 0;
		output.w = 0;
		output.h = 0;
	}
	return output;

}

int ska::RectangleUtils::getDirectionFromPos(const Point<float>& center, const Point<float>& destination) {
	double pente;

	if (destination.x != center.x) {
		pente = NumberUtils::absolute((destination.y - center.y) / (static_cast<double>(destination.x) - center.x));
	} else {
		if (destination.y > center.y){
			return 0;
		} else if (destination.y < center.y) {
			return 2;
		} else {
			return -1;
		}
	}

	if (pente < 0.0) {
		pente *= -1;
	}

	int direction;
	//taux d'accroissement entre (TAILLEECRAN;TAILLEECRAN/3) et (0; TAILLEECRAN*2/3)
	const double penteDiago1 = 1. / 3;
	//taux d'accroissement entre (TAILLEECRAN*2/3; 0) et (TAILLEECRAN/3;TAILLEECRAN)
	const double penteDiago2 = 3.;

	if (pente > penteDiago2) {
		if (destination.y > center.y) {
			direction = 0;
		} else {
			direction = 2;
		}
	} else if (pente > penteDiago1) {
		if (destination.x > center.x && destination.y > center.y) {
			direction = 4;
		} else if (destination.x > center.x && destination.y < center.y) {
			direction = 5;
		} else if (destination.x < center.x && destination.y < center.y) {
			direction = 6;
		} else {
			direction = 7;
		}

	} else {
		if (destination.x > center.x) {
			direction = 1;
		} else {
			direction = 3;
		}
	}

	return direction;
}

ska::Rectangle ska::RectangleUtils::posToCenterPicture(const Rectangle& imageToCenter, const Rectangle& imageBackground) {
	Rectangle posCenter;
	posCenter.x = imageBackground.x + (imageBackground.w + 1) / 2 - (imageToCenter.w + 1) / 2;
	posCenter.y = imageBackground.y + (imageBackground.h + 1) / 2 - (imageToCenter.h + 1) / 2;
	posCenter.h = imageToCenter.h;
	posCenter.w = imageToCenter.w;

	return posCenter;
}

ska::RectangleUtils::~RectangleUtils() {}
