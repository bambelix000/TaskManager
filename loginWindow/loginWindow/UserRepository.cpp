#include "UserRepository.h"



static std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, sep)) {
        result.push_back(item);
    }
    return result;
}

void UserRepository::updateUser(int userId, const UserAggregate& updatedAggregate) {
    usersMap[userId] = updatedAggregate;
}

void UserRepository::loadAll(const std::string& usersFile,
    const std::string& tasksFile,
    const std::string& availabilityFile) {
    using namespace std;
    namespace fs = std::filesystem;
    usersMap.clear();

    std::map<int, std::string> pathsMap = {
    {1, usersFile},
    {2, tasksFile},
    {3, availabilityFile}
    };

    for (const auto& [key, path] : pathsMap) {
        fs::path filePath(path);
        fs::path parent = filePath.parent_path();
        if (!parent.empty() && !fs::exists(parent)) {
            fs::create_directories(parent);
        }
    }

        ifstream file(usersFile);
        string line;
        getline(file, line);
        while (getline(file, line)) {
            vector<string> parts = split(line, ';');
            if (parts.size() < 7) continue;
            int id = stoi(parts[0]);
            string login = parts[4], password = parts[5], role = parts[6];
            string firstName = parts[1], lastName = parts[2], email = parts[3];

            shared_ptr<User> user;
            if (role == "admin") {
                user = make_shared<Admin>(id, login, password, firstName, lastName, email);
            }
            else {
                user = make_shared<Employee>(id, login, password, firstName, lastName, email);
            }
            usersMap[id] = UserAggregate(user, {}, Schedule(id));
        }
    
        ifstream tfile(tasksFile);
        string lineu;
        while (getline(tfile, lineu)) {
            if (lineu.empty() || lineu.rfind("NAZWA", 0) == 0) continue;
            vector<string> parts = split(lineu, ';');
            if (parts.size() < 7) continue;
            int assignedId = stoi(parts[3]);
            Task task(parts[0], parts[1], parts[2], parts[3], parts[4], parts[5] == "1", parts[6]);
            if (usersMap.count(assignedId)) {
                usersMap[assignedId].tasks.push_back(task);
            }
        }

        ifstream afile(availabilityFile);
        string lines;
        while (getline(afile, lines)) {
            if (lines.empty()) continue;
            vector<string> parts = split(lines, ';');
            if (parts.size() < 3) continue;
            int id = stoi(parts[0]);
            string day = parts[1];
            vector<string> hourStrs = split(parts[2], ',');
            vector<int> hours;
            for (const auto& h : hourStrs) {
                if (!h.empty()) hours.push_back(stoi(h));
            }
            if (usersMap.count(id)) {
                usersMap[id].schedule.setAvailability(day, hours);
            }
        }
    for (auto& [id, aggregate] : usersMap) {
        Schedule* schedule = &aggregate.schedule;
        if (schedule) {
            schedule->clearOldAvailability(6);
        }
    }
}

void UserRepository::saveAll(const std::string& usersFile,
    const std::string& tasksFile,
    const std::string& availabilityFile) {
    namespace fs = std::filesystem;

    std::map<int, std::string> pathsMap = {
		{1, usersFile},
		{2, tasksFile},
		{3, availabilityFile}
    };

    for (const auto& [key, path] : pathsMap) {
        fs::path filePath(path);
        fs::path parent = filePath.parent_path();
        if (!parent.empty() && !fs::exists(parent)) {
            fs::create_directories(parent);
        }
    }
    
    std::ofstream ufile(usersFile, std::ios::out | std::ios::trunc);
    if (ufile.is_open()) {
        ufile << "ID;Imiê;Nazwisko;Email;Login;Has³o;Rola\n";
        for (const auto& pair : usersMap) {
            const auto& user = pair.second.user;
            ufile << user->getId() << ";"
                << user->getFirstName() << ";"
                << user->getLastName() << ";"
                << user->getEmail() << ";"
                << user->getLogin() << ";"
                << user->getPassword() << ";"
                << user->getRole() << "\n";
        }
    }
    
    std::ofstream tfile(tasksFile, std::ios::out | std::ios::trunc);
    if (tfile.is_open()) {
        for (const auto& pair : usersMap) {
            for (const auto& task : pair.second.tasks) {
                tfile << task.getTitle() << ";"
                    << task.getDescription() << ";"
                    << task.getDeadline() << ";"
                    << task.getAssignedTo() << ";"
                    << task.getCreatedDate() << ";"
                    << (task.getStatus() ? "1" : "0") << ";"
                    << task.getPriority() << "\n";
            }
        }
    }
    
    std::ofstream afile(availabilityFile, std::ios::out | std::ios::trunc);
    if (afile.is_open()) {
        for (const auto& pair : usersMap) {
            const auto& schedule = pair.second.schedule;
            for (const auto& entry : schedule.getAllAvailability()) {
                afile << schedule.getUserId() << ";"
                    << entry.first << ";";
                for (size_t i = 0; i < entry.second.size(); ++i) {
                    afile << entry.second[i];
                    if (i + 1 < entry.second.size())
                        afile << ",";
                }
                afile << "\n";
            }
        }
    }
    
}


UserAggregate* UserRepository::getUserAggregate(int userId) {
    auto it = usersMap.find(userId);
    if (it != usersMap.end()) return &it->second;
    return nullptr;
}

std::vector<Task>* UserRepository::getUserTasks(int userId) {
    auto it = usersMap.find(userId);
    if (it != usersMap.end()) return &it->second.tasks;
    return nullptr;
}

Schedule* UserRepository::getUserSchedule(int userId) {
    auto it = usersMap.find(userId);
    if (it != usersMap.end()) return &it->second.schedule;
    return nullptr;
}

int UserRepository::findUserIdByLogin(const std::string& login) const {
    for (const auto& pair : usersMap) {
        if (pair.second.user && pair.second.user->getLogin() == login)
            return pair.first;
    }
    return -1;
}

User* UserRepository::getUserById(int userId) {
    auto it = usersMap.find(userId);
    if (it != usersMap.end()) return it->second.user.get();
    return nullptr;
}