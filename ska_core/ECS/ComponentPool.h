#pragma once
#include <string>
#include "ECSDefines.h"

namespace ska {
	/**
	 * \brief Contains components
	 */
	class ComponentPool {
	public:
		ComponentPool() = default;
		virtual ~ComponentPool() = default;
	
		virtual unsigned int remove(const EntityId& entityId) = 0;
		virtual unsigned int addEmpty(const EntityId& entityId) = 0;

		virtual std::string serialize(const EntityId& entityId, const std::string& field) = 0;
		virtual void deserialize(const EntityId& entityId, const std::string& field, const std::string& value) = 0;
	};
}
