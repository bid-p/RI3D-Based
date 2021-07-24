#include "subsystems/intake.hpp"
#include "main.h"

ControllerButton intakeReleaseBtn = controller[ControllerDigital::A];

namespace intake {

intakeStates currState = off;

pros::ADIDigitalOut intakeSolenoid(INTAKE_PORT, true);

void task(void* param) {

    while (true) {
        update();

        act();

        pros::delay(10);
    }
}

void update() {
    currState = off;
    pros::lcd::print(5, "intake state: %c", currState);
    // printf("intake state: %c | intake Enc: %d\n", currState, (int)intake1.getPosition());
    if (intakeReleaseBtn.isPressed()) {
        currState = on;
    }
}

void act() {

    switch (currState) {
    case off:
        intakeSolenoid.set_value(true);
        break;

    case on:
        intakeSolenoid.set_value(false);
        break;
    }
}

} // namespace intake