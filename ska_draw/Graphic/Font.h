#pragma once
#include "SDLFont.h"
#include "Utils/ManagedResource.h"

namespace ska {
	class Font : public ManagedResource<SDLFont, int> {

	public:
		Font() = default;
		explicit Font(unsigned int fontSize);
		TTF_Font* getInstance() const;
		virtual ~Font() = default;

	};
}

