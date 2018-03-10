#pragma once
#include <functional>
#include <unordered_map>
#include "ComponentHandler.h"

namespace ska {
	
	/**
	 * \brief Indicates that the component inherited will be able to be serialized and deserialized from a character string.
	 * For example, it allows to do component instantiation from the scripting part of the engine.
	 */
	template <class Component>
	class SerializeComponent {
		using FieldSerializer = std::function<std::string(const Component& component)>;
	public:
		SerializeComponent() = delete;
		virtual ~SerializeComponent() = default;

		static std::string serialize(const Component& component, const std::string& field);
		static void addFieldSerializer(FieldSerializer serializer, const std::string& fieldName);

	private:
		static std::unordered_map<std::string, FieldSerializer> m_fieldSerializer;
	};

	template <class Component>
	std::string SerializeComponent<Component>::serialize(const Component& component, const std::string& field) {
		const auto fullFieldName = ComponentHandler<Component>::getClassName() + "." + field;
		if (m_fieldSerializer.find(fullFieldName) != m_fieldSerializer.end()) {
			return (m_fieldSerializer[fullFieldName])(component);
		}
		return "";
	}

	template<class Component>
	void SerializeComponent<Component>::addFieldSerializer(FieldSerializer serializer, const std::string & fieldName) {
		const auto fullFieldName = ComponentHandler<Component>::getClassName() + "." + fieldName;
		m_fieldSerializer.emplace(fullFieldName, serializer);
	}

	template <class Component>
	std::unordered_map<std::string, std::function<std::string(const Component& component)>> SerializeComponent<Component>::m_fieldSerializer;
}
