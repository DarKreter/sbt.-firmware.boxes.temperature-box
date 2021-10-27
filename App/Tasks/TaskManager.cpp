#include <TaskManager.hpp>
#include <FreeRTOS.h>
#include <queue.h>
#include <functional>

Task::Task(const Task::NameType name, size_t periodicity, size_t priority)
    : name(name), periodicity(periodicity), priority(priority){

}

void Task::executeTask() {
    initialize();

    while (true){
        run();
        vTaskDelay(periodicity);
    }
}

const char* Task::getName() const {
    return "Test";//name.c_str();
}

size_t Task::getPriority() const {
    return priority;
}

std::vector<std::shared_ptr<Task>> TaskManager::tasks;

void TaskManager::registerTask(std::shared_ptr<Task> task) {
    tasks.push_back(task);
}

void TaskManager::registerTasks() {
}

void TaskManager::startTasks() {
    // This method simply calls main task's method that executes that task
    static const auto taskEntryPoint = [](void* task){
        reinterpret_cast<Task*>(task)->executeTask();
    };

    // Create all task by calling executeTask(). This is done by passing task pointer to taskEntryPoint.
    // TODO: Add stack size as a parameter
    for(const auto& task : tasks){
        xTaskCreate(taskEntryPoint, task->getName(), 128, task.get(), task->getPriority(), nullptr);
    }
}

void TaskManager::startRtos() {
    vTaskStartScheduler();
}
