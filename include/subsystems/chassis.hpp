#pragma once

#include "main.h"

extern bool dmdToggleState;

namespace chassis {

enum chassisStates {
    notRunning = 'x',
    running = 'r',
    dmdOut = 'd',
    dmdIn = 'i',
    yield = 'y'
};
extern chassisStates currState;

enum transStates {
    speed = false,
    torque = true
};
extern transStates transState;

enum dmdStates {
    in,
    out
};
extern dmdStates dmdState;

extern Motor chasR1, chasR2, chasR3, chasL1, chasL2, chasL3;

extern pros::ADIDigitalOut transSolenoid;

extern std::shared_ptr<ChassisController> chassisController;
extern std::shared_ptr<ChassisController> noDmdChassisController;

extern void update();
extern void act();
extern void task(void* param);

} // namespace chassis
