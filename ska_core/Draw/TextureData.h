#pragma once
#include <string>
#include "Color.h"
#include "../Rectangle.h"

namespace ska {
    
	enum EnumTextureType {
		SPRITE,
		TEXT,
		RECT
	};

	/**
	 * \brief A key used to index every SDL_Texture in the memory map (see ManagedResource for more information)
	 */
	class TextureData {
	public:
		TextureData(const std::string& texturePath, Color col, const ska::Rectangle& rect, EnumTextureType type, unsigned int fs);
        TextureData();

		std::pair<std::string, Color> getData() const;

		virtual ~TextureData() = default;

		EnumTextureType type;
		unsigned int fontSize;
		ska::Rectangle rect;

	private:
		std::pair<std::string, Color> m_data;
	};
}


namespace std {
	template <>
	struct hash<ska::TextureData> {
		size_t operator()(const ska::TextureData& k) const {
			using std::size_t;
			using std::hash;

			auto d = k.getData();
			auto hashSeed = hash<string>()(d.first);
			ska::NumberUtils::hashCombine<ska::Color>(hashSeed, d.second);
			ska::NumberUtils::hashCombine<unsigned int>(hashSeed, k.fontSize);
			ska::NumberUtils::hashCombine<int>(hashSeed, static_cast<int>(k.type));
			return hashSeed;
		}
	};

	template <>
	struct equal_to<ska::TextureData> {
		bool operator()(const ska::TextureData& t, const ska::TextureData& k) const {
			return t.getData() == k.getData() && t.type == k.type && t.fontSize == k.fontSize;
		}
	};
}
