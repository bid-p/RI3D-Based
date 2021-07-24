#include "subsystems/chassis.hpp"
#include "main.h"

ControllerButton transToggleBtn = controller[ControllerDigital::A];

ControllerButton dmdOutBtn = controller[ControllerDigital::up];
ControllerButton dmdInBtn = controller[ControllerDigital::down];

namespace chassis {

chassisStates currState = notRunning;
transStates transState = speed;

Motor chasR1(CHAS_R1, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
Motor chasR2(CHAS_R2, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
Motor chasR3(CHAS_R3, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
Motor chasL1(CHAS_L1, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
Motor chasL2(CHAS_L2, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
Motor chasL3(CHAS_L3, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());

pros::ADIDigitalOut transSolenoid(TRANS_PORT, true);

okapi::MotorGroup rightMotorGroup({-CHAS_R1, CHAS_R2, CHAS_R3});
okapi::MotorGroup leftMotorGroup({CHAS_L1, -CHAS_L2, -CHAS_L3});

okapi::MotorGroup rightGroupNODMD({-CHAS_R1, CHAS_R2});
okapi::MotorGroup leftGroupNODMD({CHAS_L1, -CHAS_L2});

std::shared_ptr<ChassisController> chassisController = ChassisControllerBuilder()
                                                           .withMotors(leftMotorGroup, rightMotorGroup)
                                                           .withDimensions(AbstractMotor::gearset::green, {{4_in, 11.5_in}, imev5GreenTPR})
                                                           .build();

// std::shared_ptr<ChassisController> noDmdChassisController = ChassisControllerBuilder()
//                                                            .withMotors(leftGroupNODMD, rightGroupNODMD)
//                                                            .withDimensions(AbstractMotor::gearset::green, {{4_in, 11.5_in}, imev5GreenTPR})
//                                                            .build();

void task(void* param) {

    while (true) {
        update();

        act();

        pros::delay(10);
    }
}

void update() {
    pros::lcd::print(3, "Chassis state: %c | Enc: %d", currState, (int)chasR3.getPosition());
    if (transToggleBtn.changedToPressed()) {
        transState = (transState == speed) ? torque : speed;
        transSolenoid.set_value(transState);
    }

    if (abs(controller.getAnalog(ControllerAnalog::leftY)) > JOY_DEADBAND ||
        abs(controller.getAnalog(ControllerAnalog::rightX)) > JOY_DEADBAND) {
        currState = running;
    }

    if (dmdOutBtn.isPressed()) {
        currState = dmdOut;
    }
    if (dmdInBtn.isPressed()) {
        currState = dmdIn;
    }
}

void act() {

    switch (currState) {
    case notRunning: {
        // chassisController->getModel()->setMaxVelocity(200);
        // chassisController->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
        // chassisController->getModel()->stop();
        chasL1.moveVoltage(0);
        chasL2.moveVoltage(0);
        chasL3.moveVoltage(0);
        chasR1.moveVoltage(0);
        chasR2.moveVoltage(0);
        chasR3.moveVoltage(0);
        break;
    }

    case running: {
        // if (dmdToggleState) {
        //     // dmd is out, do thing
        //     noDmdChassisController->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
        //                                           controller.getAnalog(ControllerAnalog::rightX));
        // } else {
        //     // dmd in
        // chassisController->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
        //                                       controller.getAnalog(ControllerAnalog::rightX));
        // }
        float rightGoal = controller.getAnalog(ControllerAnalog::leftY) - controller.getAnalog(ControllerAnalog::rightX);
        float leftGoal = controller.getAnalog(ControllerAnalog::leftY) + controller.getAnalog(ControllerAnalog::rightX);
        printf("rightGoal: %d | leftGoal: %d \n", rightGoal, leftGoal);
        if (!dmdToggleState) {
            chasL1.moveVoltage(leftGoal * 12000);
            chasL2.moveVoltage(leftGoal * 12000);
            chasL3.moveVoltage(leftGoal * 12000);
            chasR1.moveVoltage(rightGoal * 12000);
            chasR2.moveVoltage(rightGoal * 12000);
            chasR3.moveVoltage(rightGoal * 12000);
        } else {
            chasL1.moveVoltage(leftGoal * 12000);
            chasL2.moveVoltage(leftGoal * 12000);
            chasR1.moveVoltage(rightGoal * 12000);
            chasR2.moveVoltage(rightGoal * 12000);
            chasL3.moveVelocity(rightGoal * 200 * 0.50);
            chasR3.moveVelocity(rightGoal * 200 * 0.50);
        }

        currState = notRunning;
        break;
    }

    case dmdOut: {
        // chassis::chassisController->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
        chassis::chasL1.moveVoltage(200);
        chassis::chasL2.moveVoltage(200);
        chassis::chasR1.moveVoltage(200);
        chassis::chasR2.moveVoltage(200);

        chassis::chasR3.moveRelative(-600, 200);
        chassis::chasL3.moveRelative(600, 200);

        currState = notRunning;
        break;
    }

    case dmdIn: {
        // chassis::chassisController->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
        chassis::chasL1.moveVoltage(-200);
        chassis::chasL2.moveVoltage(-200);
        chassis::chasR1.moveVoltage(-200);
        chassis::chasR2.moveVoltage(-200);

        chassis::chasR3.moveRelative(600, 100);
        chassis::chasL3.moveRelative(-600, 100);

        currState = notRunning;
        break;
    }

    case yield: {
        break;
    }
    }
}

} // namespace chassis