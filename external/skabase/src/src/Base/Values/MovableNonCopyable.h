#pragma once

namespace ska {
	struct MovableNonCopyable {
		MovableNonCopyable() = default;

		MovableNonCopyable(const MovableNonCopyable&) = delete;
		MovableNonCopyable& operator=(const MovableNonCopyable&) = delete;

		MovableNonCopyable(MovableNonCopyable&&) noexcept = default;
		MovableNonCopyable& operator=(MovableNonCopyable&&) = default;

		virtual ~MovableNonCopyable() = default;
	};
}