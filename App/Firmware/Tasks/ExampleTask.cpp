#include "ExampleTask.hpp"

// Create a task named "ExampleTask" with priority 3, periodicity 1000ms and
// custom stack size 256.
ExampleTask::ExampleTask()
    : SBT::System::PeriodicTask("ExampleTask", 3, 1000, 256)
{
}

// This task does not need any initialization
void ExampleTask::initialize() {}

void ExampleTask::run() {}
