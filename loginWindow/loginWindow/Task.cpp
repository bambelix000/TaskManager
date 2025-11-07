#include "Task.h"


Task::Task(const std::string& title, const std::string& description, const std::string& deadline,
           const std::string& assignedTo, const std::string& createdDate, bool status, const std::string& priority)
    : title(title), description(description), deadline(deadline), assignedTo(assignedTo), 
      createdDate(createdDate), status(status), priority(priority) {}

std::string Task::getTitle() const { 
    return title;
}
std::string Task::getDescription() const { 
    return description; 
}
std::string Task::getDeadline() const { 
    return deadline; 
}
bool Task::getStatus() const { 
    return status; 
}
std::string Task::getAssignedTo() const { 
    return assignedTo; 
}
std::string Task::getCreatedDate() const { 
    return createdDate; 
}
std::string Task::getPriority() const { 
    return priority; 
}

void Task::setStatus(bool ready) { 
    status = ready; 
}

Task Task::fromString(const std::string& line) {
    std::stringstream ss(line);
    std::string title, description, deadline, assignedTo, createdDate, statusStr, priority;
    std::getline(ss, title, ';');
    std::getline(ss, description, ';');
    std::getline(ss, deadline, ';');
    std::getline(ss, assignedTo, ';');
    std::getline(ss, createdDate, ';');
    std::getline(ss, statusStr, ';');
    std::getline(ss, priority, ';');
    bool status = (statusStr == "1");
    return Task(title, description, deadline, assignedTo, createdDate, status, priority);
}

std::string Task::toString() const {
    return title + ";" + description + ";" + deadline + ";" + assignedTo + ";" +
           createdDate + ";" + (status ? "1" : "0") + ";" + priority;
}
