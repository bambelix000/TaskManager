#pragma once
#include <QMainWindow>
#include <memory>
#include "Employee.h"
#include "Task.h"
#include <QDate>
#include <QTableWidgetItem>  
#include <QString>
#include <vector>
#include <QWidget>            
#include <QCheckBox>         
#include <QHBoxLayout>        
#include <QCloseEvent>
#include <regex>
#include <QTimeEdit>
#include <ranges>
#include <algorithm>

namespace Ui {
    class EmployeeWindow;
}

class EmployeeWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EmployeeWindow(std::shared_ptr<Employee> employee, QWidget* parent = nullptr);
    ~EmployeeWindow();

private slots:
    // sloty obs³uguj¹ce zdarzenia (opisane w nazwach funkcji)
    void on_buttonSaveSchedule_clicked();
    void onTaskCheckBoxStateChanged(int state);
    void logout();
    void updateScheduleDays();
    QString getTaskDescriptionForEmployee(const QString& title);
    void on_tableWidgetTasks_itemDoubleClicked(QTableWidgetItem* item);

private:
    Ui::EmployeeWindow* ui;
    std::shared_ptr<Employee> employee;
    QDate currentStartDate;
    int scheduleOffset = 0;
    int expandedRow = -1;
    // dœwie¿a listê zadañ pracownika.
    void refreshTasks();
    // Odœwie¿a dostêpnoœæ pracownika.
    void refreshAvailability();
    // Sortuje zadania wed³ug wybranej opcji.
    void sortTasks(std::vector<Task>& tasks, const QString& sortOption);
    // Przechodzi do poprzednich dni w grafiku.
    void on_btnPrevDays_clicked();
    // Przechodzi do nastêpnych dni w grafiku.
    void on_btnNextDays_clicked();
    // Obs³uguje zdarzenie zamkniêcia okna pracownika.
    void closeEvent(QCloseEvent* event) override;
};
