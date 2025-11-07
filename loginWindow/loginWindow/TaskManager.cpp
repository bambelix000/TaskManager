#include "TaskManager.h"


TaskManager::TaskManager(const std::string& filePath)
    : filePath("tasks.txt") {
    loadTasks();
}

void TaskManager::loadTasks() {
    tasks.clear();
    std::ifstream fin(filePath);
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line.find(';') == std::string::npos) continue;
        tasks.push_back(Task::fromString(line));
    }
}

void TaskManager::saveTasks() const {
    std::ofstream fout(filePath, std::ios::trunc);
    for (const auto& task : tasks) {
        fout << task.toString() << "\n";
    }
}

void TaskManager::addTask(const Task& task) {
    tasks.push_back(task);
    saveTasks();
}

std::vector<Task> TaskManager::searchTasks(const std::string& filter) const {
    std::vector<Task> result;
    for (const auto& task : tasks) {
        if (task.getTitle().find(filter) != std::string::npos ||
            task.getAssignedTo().find(filter) != std::string::npos) {
            result.push_back(task);
        }
    }
    return result;
}

const std::vector<Task>& TaskManager::getTasks() const {
    return tasks;
}

void TaskManager::setTaskStatus(size_t index, bool ready) {
    if (index < tasks.size()) {
        tasks[index].setStatus(ready);
        saveTasks();
    }
}

