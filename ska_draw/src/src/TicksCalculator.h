#pragma once
#include <array>
#include "FpsRenderable.h"

namespace ska {
	class TicksCalculator {
	public:
		TicksCalculator();
		double calculate(long newtick);
		~TicksCalculator() = default;

	private:
		const static int MAXSAMPLES = 100;
		int m_tickindex;
		double m_ticksum;
		std::array<int, MAXSAMPLES> m_ticklist;
	};
}
