#include "Employee.h"


extern UserRepository g_userRepo;

Employee::Employee(int id, const std::string& login, const std::string& password,
    const std::string& firstName, const std::string& lastName, const std::string& email)
    : User(id, login, password, "employee", firstName, lastName, email) {
}

void Employee::changeTaskStatus(const std::string& timestamp, bool newStatus) {
    UserAggregate* agg = g_userRepo.getUserAggregate(this->getId());
    if (!agg) return;
    for (auto& task : agg->tasks) {
        if (task.getCreatedDate() == timestamp) {
            task.setStatus(newStatus);
            break;
        }
    }
}

void Employee::setAvailability(const std::string& day, const std::vector<int>& hours) {
    UserAggregate* agg = g_userRepo.getUserAggregate(this->getId());
    if (!agg) return;
    agg->schedule.setAvailability(day, hours);
}

QString Employee::getWindowTitle() const {
    return "Okno pracownika: " + QString::fromStdString(firstName) + " " + QString::fromStdString(lastName);
}
