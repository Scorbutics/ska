#include "TileAgglomerate.h"
#include "TileWorld.h"

std::vector<ska::Rectangle> ska::TileAgglomerate::apply(const ska::TileWorld& world, bool horizontal) {
	const int blockSize = world.getBlockSize();
	const auto width = world.getNbrBlocX();
	const auto height = world.getNbrBlocY();
	const auto area = width * height;

	const auto dimension = horizontal ? width : height;

	std::vector<ska::Rectangle> layer;
	auto lastCol = false;
	for (auto i = 0; i < area; i++) {
		if(i % dimension == 0) {
			lastCol = false;
		}

		const auto x = horizontal ? i % width : i / height;
		const auto y = horizontal ? i / width : i % height;
		const auto col = world.getCollision(x, y);
		if (col) {
			if (!lastCol) {
				layer.push_back(ska::Rectangle{ static_cast<int>(x * blockSize), static_cast<int>(y * blockSize), blockSize, blockSize });
			} else {
				auto& last = layer.back();
				if (horizontal) {
					last.w += blockSize;
				} else {
					last.h += blockSize;
				}
			}
		}

		lastCol = col;
	}
	return layer;
}
