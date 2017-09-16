#pragma once
#include <string>
#include "Color.h"

namespace ska {
    class SDLRenderer;

	/**
	 * \brief A key used to index every SDL_Texture in the memory map (see ManagedResource for more information)
	 */
	class TextureData {
	public:
		TextureData(SDLRenderer& r, const std::string&, Color c, bool text, unsigned int fontSize);
        TextureData();

		std::pair<std::string, Color> getData() const;
		const SDLRenderer& getRenderer() const;

		virtual ~TextureData() = default;

		bool text;
		unsigned int fontSize;

	private:
		std::pair<std::string, Color> m_data;
		SDLRenderer* m_renderer;		
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
			ska::NumberUtils::hashCombine<bool>(hashSeed, k.text);
			return hashSeed;
		}
	};

	template <>
	struct equal_to<ska::TextureData> {
		bool operator()(const ska::TextureData& t, const ska::TextureData& k) const {
			return t.getData() == k.getData() && t.text == k.text && t.fontSize == k.fontSize;
		}
	};
}
