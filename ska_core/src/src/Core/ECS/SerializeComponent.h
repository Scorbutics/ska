#pragma once
#include <functional>
#include <unordered_map>
#include "Component.h"
#include "../Exceptions/IllegalStateException.h"

namespace ska {

	/**
	 * \brief Indicates that the component will be able to be serialized and deserialized from a character string.
	 * For example, it allows to do component instantiation from the scripting part of the engine.
	 */
	template <class Component>
	class SerializeComponent {
		using FieldSerializer = std::function<std::string(const Component& component)>;
		using FieldDeserializer = std::function<void(Component& component, const std::string& value)>;
	public:
		SerializeComponent() = delete;
		virtual ~SerializeComponent() = default;

		static std::string serialize(const Component& component, const std::string& field);
		static void deserialize(Component& component, const std::string& field, const std::string& value);

		static void addFieldSerializer(FieldSerializer serializer, const std::string& fieldName);
		static void addFieldDeserializer(FieldDeserializer deserializer, const std::string& fieldName);

	private:
		static std::unordered_map<std::string, FieldSerializer> m_fieldSerializer;
		static std::unordered_map<std::string, FieldDeserializer> m_fieldDeserializer;
	};

	template <class ComponentType>
	std::string SerializeComponent<ComponentType>::serialize(const ComponentType& component, const std::string& fieldName) {
		const auto fullFieldName = Component<ComponentType>::TYPE_NAME() + "." + fieldName;
		if (m_fieldSerializer.find(fullFieldName) != m_fieldSerializer.end()) {
			return (m_fieldSerializer[fullFieldName])(component);
		}
		throw ska::IllegalStateException(("Unable to find a correctly initialized component serializer for the field " + fullFieldName).c_str());
	}

	template <class ComponentType>
	void SerializeComponent<ComponentType>::deserialize(ComponentType& component, const std::string& fieldName, const std::string& value) {
		const auto fullFieldName = Component<ComponentType>::TYPE_NAME() + "." + fieldName;
		if (m_fieldDeserializer.find(fullFieldName) != m_fieldDeserializer.end()) {
			m_fieldDeserializer[fullFieldName](component, value);
		} else {
			throw ska::IllegalStateException(("Unable to find a correctly initialized component deserializer for the field " + fullFieldName).c_str());
		}
	}

	template<class ComponentType>
	void SerializeComponent<ComponentType>::addFieldSerializer(FieldSerializer serializer, const std::string& fieldName) {
		const auto fullFieldName = Component<ComponentType>::TYPE_NAME() + "." + fieldName;
		m_fieldSerializer.emplace(std::move(fullFieldName), serializer);		
	}

	template<class ComponentType>
	void SerializeComponent<ComponentType>::addFieldDeserializer(FieldDeserializer deserializer, const std::string& fieldName) {
		const auto fullFieldName = Component<ComponentType>::TYPE_NAME() + "." + fieldName;
		m_fieldDeserializer.emplace(std::move(fullFieldName), deserializer);
	}

	template <class Component>
	std::unordered_map<std::string, typename SerializeComponent<Component>::FieldSerializer> SerializeComponent<Component>::m_fieldSerializer;

	template <class Component>
	std::unordered_map<std::string, typename SerializeComponent<Component>::FieldDeserializer> SerializeComponent<Component>::m_fieldDeserializer;

}
