#pragma once
#include "User.h"
#include <string>

class Admin : public User {
public:
    Admin(int id, const std::string& login, const std::string& password,
        const std::string& firstName, const std::string& lastName, const std::string& email);
    
    // Tworzy nowe zadanie i przypisuje je do u¿ytkownika.
    void newTask(int assignedUserId,
        const std::string& title,
        const std::string& description,
        const std::string& deadline,
        const std::string& priority);

    // Tworzy nowego u¿ytkownika o podanych danych.
    bool newUser(const std::string& login,
        const std::string& hashedPassword,
        const std::string& role,
        const std::string& firstName,
        const std::string& lastName,
        const std::string& email,
        std::string* errorMsg = nullptr);

    // Zwraca tytu³ okna dla administratora.
    QString getWindowTitle() const override;
};
