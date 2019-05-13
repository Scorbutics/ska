#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "InputComponent.h"
#include "../../SerializeComponent.h"

ska::InputComponent::InputComponent(): movePower(1.F), jumpPower(1.F){
	static auto initialized = false;
	if (!initialized) {
		initialized = true;
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeJumpPower, "jumpPower");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeMovePower, "movePower");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeJumpPower, "jumpPower");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeMovePower, "movePower");
	}
}

std::size_t ska::ComponentTag<ska::InputComponent>::m_id = static_cast<std::size_t>(-1);
