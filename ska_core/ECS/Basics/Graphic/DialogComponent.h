#pragma once
#include "../../../Data/Events/GUIEvent.h"

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
