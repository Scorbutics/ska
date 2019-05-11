#pragma once
#include "ScriptSleepComponent.h"
#include "../../../Point.h"
#include "ScriptGlobalData.h"

namespace ska {

    class ScriptPositionedGetter {
    public:
        ScriptPositionedGetter() = default;
        virtual ~ScriptPositionedGetter() = default;

        virtual ScriptPack getScripts(const Point<int>& oldCenterPos, const ska::Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection = nullptr) = 0;
		virtual ScriptGlobalPtrPack getScriptsAuto() = 0;
    };
}

