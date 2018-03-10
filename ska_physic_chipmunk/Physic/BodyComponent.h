#pragma once
#include "Space.h"

namespace ska {
	namespace cp {
		struct BodyComponent : SerializableComponent {
			BodyComponent()
				: bodyIndex(0), space(nullptr){
			}

			explicit BodyComponent(Space& space)
				: bodyIndex(0), space(&space){
			}

			std::size_t bodyIndex{};
			Space* space{};
		};
	}
}
