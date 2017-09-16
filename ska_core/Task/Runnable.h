#pragma once

namespace ska {
	/**
	 * \brief Forces the implementation of the operator "()", meaning that sub-classes could be considered as functions.
	 */
	class Runnable {
	public:
		Runnable() = default;
		virtual ~Runnable() = default;
		virtual bool operator()() = 0;
	};
}
