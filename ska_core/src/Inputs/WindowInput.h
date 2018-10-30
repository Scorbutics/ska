#pragma once

#include <cstdint>

namespace ska {
	class WindowInput {

	public:
		WindowInput();
		~WindowInput() = default;
		void setSize(int32_t data1, int32_t data2);
		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		unsigned int m_width;
		unsigned int m_height;
	};
}
