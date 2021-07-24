#include "macros.hpp"
#include "main.h"
#include "subsystems/chassis.hpp"
#include "subsystems/mogo.hpp"

using namespace okapi;

ControllerButton dmdToggleBtn = controller[ControllerDigital::L1];
ControllerButton mogoToggleBtn = controller[ControllerDigital::L2];

bool dmdToggleState = false; // in is false, out is true

namespace macro {

bool mogoToggleState = false; // in is false, out is true

int mogoInVal = 0;
int mogoOutVal = -900;

int dmdOutVal = 0;

macroStates currState = none;

void update() {
    printf("Macro state: %c | Mogo Enc: %d\n", currState, mogo::mogo1.getPosition());
    printf("dmdstate = %i", dmdToggleState);
    currState = none;
    if (mogoToggleBtn.changedToPressed()) {
        mogoToggleState = !mogoToggleState;

        currState = mogoToggle;
    }
    if (dmdToggleBtn.changedToPressed()) {
        dmdToggleState = !dmdToggleState;

        currState = dmdToggle;
    }
} // namespace macro

void act() {
    switch (currState) {
    case none: // macro is not activated
        break;
    case mogoToggle:
        mogo::currState = mogo::yield;
        if (mogoToggleState) {
            mogo::mogo1.moveAbsolute(mogoInVal, 100);
        } else {
            mogo::mogo1.moveAbsolute(mogoOutVal, 100);
        }
        break;

    case dmdToggle:
        chassis::currState = chassis::yield;
        // chassis::chassisController->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);

        if (dmdToggleState) {
            chassis::chasL1.moveVoltage(1000);
            chassis::chasL2.moveVoltage(1000);
            chassis::chasR1.moveVoltage(1000);
            chassis::chasR2.moveVoltage(1000);

            chassis::chasR3.moveRelative(-600, 200);
            chassis::chasL3.moveRelative(-600, 200);
        } else {
            chassis::chasL1.moveVoltage(-1000);
            chassis::chasL2.moveVoltage(-1000);
            chassis::chasR1.moveVoltage(-1000);
            chassis::chasR2.moveVoltage(-1000);

            chassis::chasR3.moveRelative(600, 50);
            chassis::chasL3.moveRelative(600, 50);
        }
        break;
    }
}

void task(void* param) {

    while (true) {
        update();

        act();

        pros::delay(10);
    }
}

void resetSubsystems() {
    chassis::currState = chassis::notRunning;
    mogo::currState = mogo::notRunning;
}

} // namespace macro