#include "subsystems/chainbar.hpp"
#include "main.h"

ControllerButton chainbarUpBtn = controller[ControllerDigital::R1];
ControllerButton chainbarDownBtn = controller[ControllerDigital::R2];

namespace chainbar {

chainbarStates currState = notRunning;

Motor chainbar1(LIFT_1, false, AbstractMotor::gearset::red, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());

void task(void* param) {

    while (true) {
        update();

        act();

        pros::delay(10);
    }
}

void update() {
    currState = notRunning;
    pros::lcd::print(4, "Chainbar state: %c | Enc: %d", currState, (int)chainbar1.getPosition());
    // printf("chainbar state: %c | chainbar Enc: %d\n", currState, (int)chainbar1.getPosition());
    if (chainbarUpBtn.isPressed()){
        currState = up;
    }
    if (chainbarDownBtn.isPressed()){
        currState = down;
    }
}

void act() {

    switch (currState) {
    case notRunning:
        chainbar1.setBrakeMode(AbstractMotor::brakeMode::coast);
        chainbar1.moveVelocity(0);
        break;

    case up:
        chainbar1.moveVelocity(100);
        break;

    case down:
        chainbar1.moveVelocity(-100);
        break;

    case hold:
        chainbar1.setBrakeMode(AbstractMotor::brakeMode::hold);
        chainbar1.moveVelocity(0);
        break;
    }
}

} // namespace chainbar