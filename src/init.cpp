#include "main.h"
#include "init.hpp"

#include "subsystems/chassis.hpp"
#include "subsystems/mogo.hpp"
#include "subsystems/chainbar.hpp"

#include "macros.hpp"

Controller controller = Controller();

pros::Task *chassisTask;
pros::Task* mogoTask;
pros::Task* chainbarTask;

pros::Task* macroTask;

void initializeAllTasks() {
    chassisTask = new pros::Task(chassis::task, NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Chassis Task");
    mogoTask = new pros::Task(mogo::task, NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Mogo Task");
    chainbarTask = new pros::Task(chainbar::task, NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Chainbar Task");

    macroTask = new pros::Task(macro::task, NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Macro Task");
}