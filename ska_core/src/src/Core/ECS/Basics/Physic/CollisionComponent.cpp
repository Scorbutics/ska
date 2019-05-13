#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "Core/__internalConfig/LoggerConfig.h"
#include "CollisionComponent.h"

ska::CollisionComponent::CollisionComponent() {
	xaxis = false;
	yaxis = false;
	origin = 0;
	target = 0;
}


template class SKA_CORE_API ska::ComponentTag<ska::CollisionComponent>;
std::size_t ska::ComponentTag<ska::CollisionComponent>::m_id = static_cast<std::size_t>(-1);
