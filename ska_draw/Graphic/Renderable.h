#pragma once

namespace ska {
	class Renderable {
	public:
		Renderable() = default;
		virtual ~Renderable() = default;
		virtual int render(int m_positionx, int m_positiony, const Rectangle* clip = nullptr) const = 0;
	private:
	};
}