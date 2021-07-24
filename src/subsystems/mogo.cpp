#include "subsystems/mogo.hpp"
#include "main.h"

//ControllerButton mogoInBtn = controller[ControllerDigital::X];
//ControllerButton mogoOutBtn = controller[ControllerDigital::B];

namespace mogo {

mogoStates currState = yield;

Motor mogo1(MOGO_1, false, AbstractMotor::gearset::red, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());

void task(void* param) {

    while (true) {
        update();

        act();

        pros::delay(10);
    }
}

void update() {
    // currState = notRunning;
    pros::lcd::print(2, "Mogo state: %c | Mogo Enc: %d", currState, (int)mogo1.getPosition());
    // printf("Mogo state: %c | Mogo Enc: %d\n", currState, (int)mogo1.getPosition());
    // if (mogoInBtn.isPressed()){
    //     currState = forward;
    // }
    // if (mogoOutBtn.isPressed()){
    //     currState = backward;
    // }
}

void act() {

    switch (currState) {
    case notRunning:
        mogo1.setBrakeMode(AbstractMotor::brakeMode::coast);
        mogo1.moveVoltage(0);
        break;

    case forward:
        mogo1.moveVelocity(100);
        break;

    case backward:
        mogo1.moveVelocity(-100);
        break;

    case yield:
        break;
    }
}

} // namespace mogo