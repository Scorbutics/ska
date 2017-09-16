#pragma once
#include <unordered_map>
#include "ComponentHandler.h"

namespace ska {
	
	class SerializableComponent;
	typedef std::string(*field_serializer_method)(const SerializableComponent& component);
	
	/**
	 * \brief Indicates that the component inherited will be able to be serialized and deserialized from a character string.
	 * For example, it allows to do component instantiation from the scripting part of the engine.
	 */
	class SerializableComponent {
	public:
		SerializableComponent() = default;
		
		const std::string serialize(const SerializableComponent& component, const std::string& field, const std::string& className);
		
		virtual ~SerializableComponent() = default;
	
	protected:
		void addFieldSerializer(field_serializer_method serializer, const std::string& fieldName, const std::string& className);

	private:
		static std::unordered_map<std::string, field_serializer_method> m_fieldSerializer;
	};
	
}
