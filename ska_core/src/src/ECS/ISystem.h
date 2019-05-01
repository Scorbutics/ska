#pragma once
#include "../Core/Priorized.h"

namespace ska {
	/**
	 * \brief Forces the implementation of an update function that refreshes the system.
	 */
	class ISystem :
	    public Priorized {
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void update(unsigned int ellapsedTime) = 0;
	};

}
