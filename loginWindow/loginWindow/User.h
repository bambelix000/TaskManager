#pragma once
#include <string>
#include <QString>

class User {
public:
    int id;
    std::string login;
    std::string password;
    std::string role;
    std::string firstName;
    std::string lastName;
    std::string email;

    User(int id, const std::string& login, const std::string& password, const std::string& role,
        const std::string& firstName, const std::string& lastName, const std::string& email);

public:
    // gettery
    int getId() const;
    std::string getLogin() const;
    std::string getPassword() const;
    std::string getRole() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getEmail() const;

    // Sprawdza poprawnoœæ loginu i has³a u¿ytkownika.
    virtual bool log(const std::string& login, const std::string& password) const;

    // Hashuje has³o u¿ytkownika.
    static std::string hashPassword(const std::string& password);

    virtual ~User();

    // Zwraca tytu³ okna odpowiedni dla u¿ytkownika.
    virtual QString getWindowTitle() const;
};