#pragma once
#include "User.h"
#include "UserRepository.h"
#include "Task.h"
#include "Schedule.h"
#include <string>
#include <vector>
#include <memory>

class Employee : public User {
public:
public:
    Employee(int id, const std::string& login, const std::string& password,
        const std::string& firstName, const std::string& lastName, const std::string& email);
    
    // Zmienia status zadania wed³ug timestampu.
    void changeTaskStatus(const std::string& timestamp, bool newStatus);
    // Ustawia dostêpnoœæ pracownika w danym dniu.
    void setAvailability(const std::string& day, const std::vector<int>& hours);
    // Zwraca tytu³ okna dla pracownika.
    QString getWindowTitle() const override;
};
