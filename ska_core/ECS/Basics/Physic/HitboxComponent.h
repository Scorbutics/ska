#pragma once

namespace ska {
	struct HitboxComponent {
	    HitboxComponent();
	    ~HitboxComponent() = default;
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}
