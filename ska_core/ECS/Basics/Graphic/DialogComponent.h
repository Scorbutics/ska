#pragma once
#include "../../SerializableComponent.h"
#include "../../../Data/Events/GUIEvent.h"

namespace ska {
	class BalloonDialog;

    class DialogComponent : public SerializableComponent {
    public:
        DialogComponent();
        virtual ~DialogComponent() = default;

	    BalloonDialog* handle;
		std::string name;
    };
}
