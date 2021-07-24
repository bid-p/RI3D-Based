#pragma once

#include "main.h"

namespace intake {

enum intakeStates {
    off = 'x',
    on = 'o'
};
extern intakeStates currState;

extern pros::ADIDigitalOut intakeSolenoid;

extern void update();
extern void act();
extern void task(void* param);

} // namespace intake