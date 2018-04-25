#pragma once
#include <string>
#include "ECSDefines.h"

namespace ska {
	/**
	 * \brief Serializes a component
	 */
	class ComponentSerializer {
	public:
		ComponentSerializer() = default;
		virtual ~ComponentSerializer() = default;

		virtual std::string getComponentField(const EntityId& id, const std::string& field) = 0;
		
		virtual unsigned int remove(const EntityId& entityId) = 0;
		virtual unsigned int addEmpty(const EntityId& entityId) = 0;
	};
}
