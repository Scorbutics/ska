#pragma once
#include "ScriptSleepComponent.h"
#include "../../../Point.h"

namespace ska {

    class ScriptPositionedGetter {
    public:
        ScriptPositionedGetter() = default;
        virtual ~ScriptPositionedGetter() = default;

        virtual std::vector<std::reference_wrapper<ScriptSleepComponent>> getScripts(const Point<int>& oldCenterPos, const ska::Point<int>& frontPos, ScriptTriggerType type, const Point<float>* normal = nullptr) = 0;
		virtual std::vector<std::reference_wrapper<ScriptSleepComponent>> getScriptsAuto() = 0;
    };
}

