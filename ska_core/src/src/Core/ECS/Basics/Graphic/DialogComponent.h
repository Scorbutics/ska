#pragma once
#include "../../../Data/Events/GUIEvent.h"
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"

namespace ska {
	class BalloonDialog;

    class DialogComponent {
    public:
        DialogComponent();
        virtual ~DialogComponent() = default;

	    BalloonDialog* handle;
		std::string name;
    };

}

SKA_DECLARE_COMPONENT(ska::DialogComponent);
