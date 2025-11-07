#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QVector>
#include <QDateTime>

struct Employee {
    int id;
    QString firstName;
    QString lastName;
    QString password; // w prostym przyk³adzie, bez hash
};

struct Task {
    QString name;
    QString description;
    QDate deadline;
    int employeeId;
    qint64 timestamp;
    bool done;
};

// Wczytanie pracowników z pliku employees.txt
QVector<Employee> loadEmployees(const QString& filename);

// Zapis pracowników do pliku
bool saveEmployees(const QString& filename, const QVector<Employee>& employees);

// Wczytanie zadañ z pliku tasks.txt
QVector<Task> loadTasks(const QString& filename);

// Zapis zadañ do pliku
bool saveTasks(const QString& filename, const QVector<Task>& tasks);

#endif // MODELS_H
