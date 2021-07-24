#include "subsystems/chassis.hpp"
#include "main.h"

ControllerButton transToggleBtn = controller[ControllerDigital::A];

ControllerButton dmdOutBtn = controller[ControllerDigital::R2];
ControllerButton dmdInBtn = controller[ControllerDigital::R1];

namespace chassis
{

    chassisStates currState = notRunning;
    transStates transState = speed;
    dmdStates dmdState = dmdStates::in;

    Motor chasR1(CHAS_R1, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
    Motor chasR2(CHAS_R2, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
    Motor chasR3(CHAS_R3, -false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees, Logger::getDefaultLogger());
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

    void task(void *param)
    {

        while (true)
        {
            update();

            act();

            pros::delay(10);
        }
    }

    void update()
    {
        pros::lcd::print(3, "Chassis state: %c | Enc: %d", currState, (int)chasR3.getPosition());
        if (transToggleBtn.changedToPressed())
        {
            transState = (transState == speed) ? torque : speed;
            transSolenoid.set_value(transState);
        }

        // if (abs(controller.getAnalog(ControllerAnalog::leftY)) > JOY_DEADBAND ||
        //     abs(controller.getAnalog(ControllerAnalog::rightX)) > JOY_DEADBAND ||
        //     abs(controller.getAnalog(ControllerAnalog::rightY)) > JOY_DEADBAND) 
        // {
            currState = running;
        // }

        // if (dmdOutBtn.isPressed())
        // {
        //     //currState = dmdOut;
        //     dmdToggleState = true;
        // }
        // if (dmdInBtn.isPressed())
        // {
        //     //currState = dmdIn;
        //     dmdToggleState = false;
        // }
    }

    void act()
    {

        switch (currState)
        {
        case notRunning:
        {
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

        case running:
        {

/*
 * Inputs: Target Drive Speed, Target Mogo Speed
 * Drive motors = Target Drive Speed
* Target Mogo Speed = Mogo motors - Drive motors; ergo
  Mogo motors = Target Mogo Speed + Drive Motors = Target Mogo Speed + Drive Speed 
  Offset = Mogo motors -  127
 */

            const float holdInMultiplier = 0.1;
            const float holdOutMultiplier = -0.1;

            float rightGoal = controller.getAnalog(ControllerAnalog::rightY); // - controller.getAnalog(ControllerAnalog::leftX);
            float leftGoal =  controller.getAnalog(ControllerAnalog::leftY);  // + controller.getAnalog(ControllerAnalog::leftX);
            int barGoal = ((dmdInBtn.isPressed() ? 1 : 0) - (dmdOutBtn.isPressed() ? 1 : 0)); // controller.getAnalog(ControllerAnalog::rightY);

            float barGoalReal = ((float) barGoal);

            if (barGoal < 0) {
                dmdState = dmdStates::out;
            } else if (barGoal > 0) {
                dmdState = dmdStates::in;
            } else {
                if (dmdState == dmdStates::out) {
                    barGoalReal = holdOutMultiplier;
                } else {
                    barGoalReal = holdInMultiplier;
                }
            }

            float dmdRightGoal = barGoalReal + rightGoal;
            float dmdLeftGoal = barGoalReal + leftGoal;

            //printf("bargoal: %f | dmdState: %d\n", barGoal, dmdState);

            if (dmdRightGoal > 1){
                rightGoal -= dmdRightGoal - 1;
                dmdRightGoal = 1;
            }
            if (dmdRightGoal < -1){
                rightGoal -= dmdRightGoal + 1;
                dmdRightGoal = -1;
            }
            if (dmdLeftGoal > 1){
                leftGoal -= dmdLeftGoal - 1;
                dmdLeftGoal = 1;
            }
            if (dmdLeftGoal < -1){
                leftGoal -= dmdLeftGoal + 1;
                dmdLeftGoal = -1;
            }

            if (abs(dmdLeftGoal) > 1 || abs(dmdRightGoal) > 1) {
                printf("dmdRight: %f | dmdLeft: %f", dmdRightGoal, dmdLeftGoal);
            }
            
            rightGoal = rightGoal > 1 ? 1 : rightGoal;
            rightGoal = rightGoal < -1 ? -1 : rightGoal;
            leftGoal = leftGoal > 1 ? 1 : leftGoal;
            leftGoal = leftGoal < -1 ? -1 : leftGoal;
            //
            //TODO(JS): remove to enable turn
            //leftGoal = rightGoal;

            printf("left: %f, right: %f, dmdLeft: %f, dmdRight: %f\n", leftGoal, rightGoal, dmdLeftGoal, dmdRightGoal);

            // chasL1.moveVoltage(leftGoal * 12000);
            // chasL2.moveVoltage(leftGoal * 12000);
            // chasR1.moveVoltage(rightGoal * 12000);
            // chasR2.moveVoltage(rightGoal * 12000);

            // chasL3.moveVoltage(dmdLeftGoal * 12000);
            // chasR3.moveVoltage(dmdRightGoal * 12000);

            chasL1.moveVelocity(leftGoal * 200);
            chasL2.moveVelocity(leftGoal * 200);
            chasR1.moveVelocity(rightGoal * 200);
            chasR2.moveVelocity(rightGoal * 200);

            chasL3.moveVelocity(dmdLeftGoal * 200);
            chasR3.moveVelocity(dmdRightGoal * 200);

/*
            float rightGoal = controller.getAnalog(ControllerAnalog::leftY) - controller.getAnalog(ControllerAnalog::rightX);
            float leftGoal = controller.getAnalog(ControllerAnalog::leftY) + controller.getAnalog(ControllerAnalog::rightX);

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
                chasL3.moveVelocity(rightGoal * 200 * 0);
                chasR3.moveVelocity(rightGoal * 200 * 0);
            }
            */

            currState = notRunning;
            break;


            // DEPRECATED
            // if (dmdToggleState) {
            //     // dmd is out, do thing
            //     noDmdChassisController->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
            //                                           controller.getAnalog(ControllerAnalog::rightX));
            // } else {
            //     // dmd in
            // chassisController->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
            //                                       controller.getAnalog(ControllerAnalog::rightX));
            // }
            // printf("rightGoal: %d | leftGoal: %d \n", rightGoal, leftGoal);
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

            //case yield: {
            //   break;
            //}
        }
    }

} // namespace chassis