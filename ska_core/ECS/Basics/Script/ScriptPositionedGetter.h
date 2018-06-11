#pragma once
#include "ScriptSleepComponent.h"
#include "../../../Point.h"

namespace ska {

    class ScriptPositionedGetter {
    public:
        ScriptPositionedGetter() = default;
        virtual ~ScriptPositionedGetter() = default;

        virtual std::vector<ScriptSleepComponent> getScripts(const Point<int>& oldCenterPos, const ska::Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection = nullptr) = 0;
		virtual std::vector<ScriptSleepComponent> getScriptsAuto() = 0;
    };
}

