#pragma once

#include "okapi/api.hpp"

using namespace okapi;

namespace macro
{

enum macroStates {
    none = 'x',
    mogoToggle = 'm',
    dmdToggle = 'd'
};
extern macroStates currState;

extern void update();

extern void act();

extern void task(void* param);

extern void resetSubsystems();

} // namespace macro