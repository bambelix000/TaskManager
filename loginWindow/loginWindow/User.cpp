#include "User.h"
#include <QString>
#include <QCryptographicHash>
#include <fstream>
#include <sstream>

User::User(int id, const std::string& login, const std::string& password, const std::string& role,
    const std::string& firstName, const std::string& lastName, const std::string& email)
    : id(id), login(login), password(password), role(role),
    firstName(firstName), lastName(lastName), email(email) {}

int User::getId() const { 
    return id; 
}
std::string User::getLogin() const { 
    return login; 
}
std::string User::getPassword() const { 
    return password; 
}
std::string User::getRole() const { 
    return role; 
}
std::string User::getFirstName() const { 
    return firstName; 
}
std::string User::getLastName() const { 
    return lastName; 
}
std::string User::getEmail() const { 
    return email; 
}

QString User::getWindowTitle() const {
    return "Okno logowania";
}


bool User::log(const std::string& login, const std::string& password) const {
    return this->login == login && this->password == User::hashPassword(password);
}

std::string User::hashPassword(const std::string& password) {
    QByteArray passwordBytes = QString::fromStdString(password).toUtf8();
    QByteArray hash = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

User::~User() {}