#pragma once
#include "Task.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

class TaskManager {
public:
    TaskManager(const std::string& filePath);

    // Wczytuje zadania z pliku.
    void loadTasks();

    // Zapisuje zadania do pliku.
    void saveTasks() const;

    // Dodaje nowe zadanie do listy.
    void addTask(const Task& task);

    // Wyszukuje zadania spe³niaj¹ce okreœlony filtr.
    std::vector<Task> searchTasks(const std::string& filter) const;

    // Zwraca sta³¹ referencjê do listy wszystkich zadañ.
    const std::vector<Task>& getTasks() const;

    // Zmienia status zadania.
    void setTaskStatus(size_t index, bool ready);



private:
    std::vector<Task> tasks;
    std::string filePath;
};
