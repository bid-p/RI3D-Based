#pragma once

#include "main.h"

namespace mogo {

enum mogoStates {
    notRunning = 'x',
    forward = 'f',
    backward = 'b',
    yield = 'y'
};
extern mogoStates currState;

extern Motor mogo1;

extern void update();
extern void act();
extern void task(void* param);

} // namespace mogo
