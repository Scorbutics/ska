#pragma once
#include <functional>
#include <unordered_map>
#include "../Exceptions/IllegalStateException.h"

namespace ska {

	template<typename C>
	struct has_getClassName {
	private:

		template<typename T>
		static constexpr auto check(T*)
			-> typename
			std::is_same<
				decltype(std::declval<T>().getClassName()),
				std::string
			>::type;

		template<typename>
		static constexpr std::false_type check(...);

		typedef decltype(check<C>(0)) type;

	public:
		static constexpr bool value = type::value;
	};

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

		static void checkComponent() {
			static_assert(has_getClassName<Component>::value, "This component class must implements a static \"getClassName\" member function in order to be serialized");
		}
	};

	template <class Component>
	std::string SerializeComponent<Component>::serialize(const Component& component, const std::string& fieldName) {
		checkComponent();

		const auto fullFieldName = Component::getClassName() + "." + fieldName;
		if (m_fieldSerializer.find(fullFieldName) != m_fieldSerializer.end()) {
			return (m_fieldSerializer[fullFieldName])(component);
		}
		throw ska::IllegalStateException(("Unable to find a correctly initialized component serializer for the field " + fullFieldName).c_str());
	}

	template <class Component>
	void SerializeComponent<Component>::deserialize(Component& component, const std::string& fieldName, const std::string& value) {
		checkComponent();

		const auto fullFieldName = Component::getClassName() + "." + fieldName;
		if (m_fieldDeserializer.find(fullFieldName) != m_fieldDeserializer.end()) {
			m_fieldDeserializer[fullFieldName](component, value);
		} else {
			throw ska::IllegalStateException(("Unable to find a correctly initialized component deserializer for the field " + fullFieldName).c_str());
		}
	}

	template<class Component>
	void SerializeComponent<Component>::addFieldSerializer(FieldSerializer serializer, const std::string& fieldName) {
		checkComponent();

		const auto fullFieldName = Component::getClassName() + "." + fieldName;
		m_fieldSerializer.emplace(std::move(fullFieldName), serializer);		
	}

	template<class Component>
	void SerializeComponent<Component>::addFieldDeserializer(FieldDeserializer deserializer, const std::string& fieldName) {
		checkComponent();

		const auto fullFieldName = Component::getClassName() + "." + fieldName;
		m_fieldDeserializer.emplace(std::move(fullFieldName), deserializer);
	}

	template <class Component>
	std::unordered_map<std::string, typename SerializeComponent<Component>::FieldSerializer> SerializeComponent<Component>::m_fieldSerializer;

	template <class Component>
	std::unordered_map<std::string, typename SerializeComponent<Component>::FieldDeserializer> SerializeComponent<Component>::m_fieldDeserializer;

}
