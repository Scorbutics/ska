#pragma once
#include <string>
#include "Draw/Color.h"
#include "Rectangle.h"

namespace ska {

	enum class EnumTextureType {
		SPRITE,
		TEXT,
		RECT,
		NONE
	};

	struct TextureDataDescriptor {
		std::string text;
		ska::Color color;
		const ska::Color* outlineColor = nullptr;
	};

	bool operator==(const ska::TextureDataDescriptor& t1, const ska::TextureDataDescriptor& t2);

	/**
	 * \brief A key used to index every SDL_Texture in the memory map (see ManagedResource for more information)
	 */
	class TextureData {
	public:
		TextureData(const std::string& texturePath, Color col, const ska::Rectangle& rect, EnumTextureType type, unsigned int fontSiz, const Color* outlineColor = nullptr);
        TextureData();

		const TextureDataDescriptor& getData() const;

		virtual ~TextureData() = default;

		EnumTextureType type;
		unsigned int fontSize;
		ska::Rectangle rect;

	private:
		TextureDataDescriptor m_data;
	};
}


namespace std {
	template <>
	struct hash<ska::TextureData> {
		size_t operator()(const ska::TextureData& k) const {
			using std::size_t;
			using std::hash;

			auto d = k.getData();
			auto hashSeed = hash<string>()(d.text);
			ska::NumberUtils::hashCombine<ska::Color>(hashSeed, d.color);
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
