#pragma once

#include "GameEventDispatcher.h"
#include "../../Utils/Observable.h"

namespace ska {
	/**
	 * \brief Variant of the GameEventDispatcher that also handles custom events
	 * \tparam ET The custom events type to be added managed by this event dispatcher
	 */
	template <class ...ET>
	class ExtensibleGameEventDispatcher :
		public ska::GameEventDispatcher,
		public ska::Observable<ET>... {
	public:
		ExtensibleGameEventDispatcher() = default;
		ExtensibleGameEventDispatcher(const ExtensibleGameEventDispatcher&) = delete;
		ExtensibleGameEventDispatcher(ExtensibleGameEventDispatcher&&) = default;
		virtual ~ExtensibleGameEventDispatcher() = default;

	private:

	};

}
