#include "Admin.h"
#include "UserRepository.h"
#include "Task.h"
#include <fstream>
#include <sstream>
#include <QDateTime>

Admin::Admin(int id, const std::string& login, const std::string& password,
             const std::string& firstName, const std::string& lastName, const std::string& email)
    : User(id, login, password, "admin", firstName, lastName, email) {}



void Admin::newTask(int assignedUserId,
    const std::string& title,
    const std::string& description,
    const std::string& deadline,
    const std::string& priority)
{
    UserAggregate* agg = userRepository->getUserAggregate(assignedUserId);

    std::string timestamp = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
    Task newTask(
        title,
        description,
        deadline,
        std::to_string(assignedUserId),
        timestamp,
        false,
        priority
    );
    agg->tasks.push_back(newTask);
}

bool Admin::newUser(const std::string& login,
    const std::string& hashedPassword,
    const std::string& role,
    const std::string& firstName,
    const std::string& lastName,
    const std::string& email,
    std::string* errorMsg)
{
    for (const auto& pair : userRepository->usersMap) {
        const auto& user = pair.second.user;
        if (user && user->getLogin() == login) {
            if (errorMsg) *errorMsg = "Login jest juz zajety.";
            return false;
        }
    }

    int lastId = 0;
    for (const auto& pair : userRepository->usersMap) {
        if (pair.first > lastId)
            lastId = pair.first;
    }
    int newId = lastId + 1;

    auto newUser = std::make_unique<User>(
        newId,
        login,
        hashedPassword,
        role,
        firstName,
        lastName,
        email
    );

    UserAggregate aggregate;
    aggregate.user = std::move(newUser);
    userRepository->usersMap[newId] = std::move(aggregate);

    return true;
}

QString Admin::getWindowTitle() const {
    return "Okno administratora: " + QString::fromStdString(firstName) + " " + QString::fromStdString(lastName);
}