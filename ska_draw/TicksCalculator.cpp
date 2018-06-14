#include <iostream>
#include "TicksCalculator.h"

ska::TicksCalculator::TicksCalculator() : m_tickindex(0), m_ticksum(0) {
	m_ticklist.fill(0);
}

double ska::TicksCalculator::calculate(long newtick) {
	/* subtract value falling off */
	m_ticksum -= m_ticklist[m_tickindex];

	/* add new value */
	m_ticksum += newtick;

	/* save new value so it can be subtracted later */
	m_ticklist[m_tickindex] = newtick;

	if (++m_tickindex == MAXSAMPLES) {
		m_tickindex = 0;
	}

	const auto average = (static_cast<double>(m_ticksum) / MAXSAMPLES);
	return (average == 0.0) ? -1.0 : (1000 / average);

}
