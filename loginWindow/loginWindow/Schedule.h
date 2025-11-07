#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <QDate>

class Schedule {
public:
    Schedule(int userId);

    // Ustawia dostêpnoœæ w danym dniu.
    void setAvailability(const std::string& day, const std::vector<int>& hours);

    // Pobiera dostêpnoœæ w danym dniu.
    std::vector<int> getAvailability(const std::string& day) const;

    // Zwraca mapê ca³ej dostêpnoœci u¿ytkownika.
    const std::map<std::string, std::vector<int>>& getAllAvailability() const;

    static Schedule fromFile(int userId, const std::string& filePath = "availability.txt");
    void saveToFile(const std::string& filePath = "availability.txt") const;

    int getUserId() const;

    // Czyœci star¹ dostêpnoœæ (pozostawia ostatnie n dni).
    void clearOldAvailability(int daysToKeep = 5);
private:
    int userId;
    std::map<std::string, std::vector<int>> availability;

};

