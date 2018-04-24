#pragma once
#include "ScriptSleepComponent.h"
#include "../../../Point.h"

namespace ska {

    class ScriptPositionedGetter {
    public:
        ScriptPositionedGetter() = default;
        virtual ~ScriptPositionedGetter() = default;

        virtual std::vector<ScriptSleepComponent*> chipsetScript(const Point<int>& oldCenterPos, const ska::Point<int>& frontPos, ScriptTriggerType type) = 0;
		virtual ScriptSleepComponent* chipsetScriptAuto() = 0;
    };
}

