#pragma once

#include "main.h"

namespace chainbar {

enum chainbarStates {
    notRunning = 'x',
    up = 'u',
    down = 'd',
    hold = 'h',
    yield = 'y'
};
extern chainbarStates currState;

extern Motor chainbar1;

extern void update();
extern void act();
extern void task(void* param);

} // namespace chainbar
