#pragma once
#include "../../../Data/Events/GUIEvent.h"
#include "Core/ECS/ComponentTag.h"
namespace ska {
	class BalloonDialog;

    class DialogComponent {
    public:
        DialogComponent();
        virtual ~DialogComponent() = default;

	    BalloonDialog* handle;
		std::string name;
    };

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<DialogComponent>;
#endif
}
