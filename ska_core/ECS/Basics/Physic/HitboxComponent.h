#pragma once

namespace ska {
	struct HitboxComponent {
	public:
	    HitboxComponent();
	    ~HitboxComponent() = default;
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}
