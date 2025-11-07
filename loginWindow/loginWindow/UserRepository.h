#pragma once
#include "UserAggregate.h"
#include "Admin.h"
#include "Employee.h"
#include <map>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

class UserRepository {
    public:
    std::map<int, UserAggregate> usersMap;

    // £aduje wszystkich u¿ytkowników, zadania oraz dostêpnoœci z plików.
    void loadAll(const std::string& usersFile = "users.txt",
        const std::string& tasksFile = "tasks.txt",
        const std::string& availabilityFile = "availability.txt");

    // Zapisuje wszystkich u¿ytkowników, zadania oraz dostêpnoœci do plików.
    void saveAll(const std::string& usersFile = "users.txt",
        const std::string& tasksFile = "tasks.txt",
        const std::string& availabilityFile = "availability.txt");

    // Zwraca wskaŸnik do agregatu u¿ytkownika po ID u¿ytkownika.
    UserAggregate* getUserAggregate(int userId);

    // Zwraca wskaŸnik do wektora zadañ u¿ytkownika o podanym ID.
    std::vector<Task>* getUserTasks(int userId);

    // Zwraca wskaŸnik do grafiku u¿ytkownika po ID.
    Schedule* getUserSchedule(int userId);

    // Wyszukuje identyfikator u¿ytkownika po loginie.
    int findUserIdByLogin(const std::string& login) const;

	// Zwraca wskaŸnik do u¿ytkownika na podstawie ID.
    User* getUserById(int userId);
    
	// Aktualizuje dane agregatu u¿ytkownika o wskazanym ID.
    void updateUser(int userId, const UserAggregate& updatedAggregate);

};

extern UserRepository g_userRepo;
extern UserRepository* userRepository;

