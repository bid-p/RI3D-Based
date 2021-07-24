#pragma once

#include "okapi/api.hpp"

using namespace okapi;

#define JOY_DEADBAND 0.08

extern Controller controller;

extern pros::Task* chassisTask;
extern pros::Task* mogoTask;
extern pros::Task* chainbarTask;

extern pros::Task* macroTask;

extern void initializeAllTasks();