#include "Schedule.h"


Schedule::Schedule(int userId) : userId(userId) {}

void Schedule::setAvailability(const std::string& day, const std::vector<int>& hours) {
    availability[day] = hours;
}

std::vector<int> Schedule::getAvailability(const std::string& day) const {
    auto it = availability.find(day);
    if (it != availability.end())
        return it->second;
    return {};
}

const std::map<std::string, std::vector<int>>& Schedule::getAllAvailability() const {
    return availability;
}

int Schedule::getUserId() const {
    return userId;
}

Schedule Schedule::fromFile(int userId, const std::string& filePath) {
    Schedule sched(userId);
    std::ifstream fin(filePath);
    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string idStr, day, hoursStr;
        std::getline(ss, idStr, ';');
        std::getline(ss, day, ';');
        std::getline(ss, hoursStr, ';');
        if (std::stoi(idStr) != userId) continue;
        std::vector<int> hours;
        std::stringstream hs(hoursStr);
        std::string hour;
        while (std::getline(hs, hour, ',')) {
            if (!hour.empty())
                hours.push_back(std::stoi(hour));
        }
        sched.setAvailability(day, hours);
    }
    return sched;
}

void Schedule::clearOldAvailability(int daysToKeep) {
    QDate threshold = QDate::currentDate().addDays(-daysToKeep);
    std::vector<std::string> toRemove;
    for (const auto& entry : availability) {
        QDate date = QDate::fromString(QString::fromStdString(entry.first), "yyyy-MM-dd");
        if (date.isValid() && date < threshold) {
            toRemove.push_back(entry.first);
        }
    }
    for (const auto& key : toRemove) {
        availability.erase(key);
    }
}