#pragma once

#include <bitset>

namespace ska {

	class KeyInput {
		constexpr static auto keysNumber = 512;
	public:
		KeyInput() = default;

		bool trigger(int key) const;
		bool toggle(int key) const;

		void setKeyState(int key, bool state);

		void resetTriggers();

		~KeyInput() = default;

	private:
		std::bitset<keysNumber> m_keys;
		std::bitset<keysNumber> m_toggle;

	};
}

