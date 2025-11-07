#pragma once

#include <QMainWindow>
#include "Admin.h"
#include "User.h"
#include "Task.h" 
#include "loginWindow.h"  
#include "UserRepository.h"  
#include <QTableWidgetItem> 
#include <QStringList>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDate> 
#include <string>
#include <QCloseEvent>
#include <QPainter>
#include <QGroupBox>
#include <QRegularExpression>
#include <ranges>
#include <algorithm>

namespace Ui {
    class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(Admin* admin, QWidget* parent = nullptr);
    ~AdminWindow();

private:
    Admin* admin;
    Ui::AdminWindow* ui;
    int scheduleOffset;
    int expandedRow = -1;
    // £aduje listê u¿ytkowników do przypisania zadañ.
    void loadUsersToAssign();

    // Obs³uguje zdarzenie zamkniêcia okna administratora.
    void closeEvent(QCloseEvent* event) override;

    // Sortuje listê zadañ wed³ug wybranej opcji.
    void sortTaskList(std::vector<Task>& tasks, const QString& sortOption);

private slots:    
    // sloty obs³uguj¹ce zdarzenia (opisane w nazwach funkcji)
    void logout();
    void onSaveTaskClicked();
    void on_btnAddEmployeeConfirm_clicked();
    void loadAndDisplayTasks(const QString& filterRegex);
    void on_btnSchedule_clicked();
    void on_btnNextDays_clicked();
    void on_btnPrevDays_clicked();
    void updateScheduleDays();
    bool eventFilter(QObject* obj, QEvent* event);
    void on_tableWidgetTasks_itemDoubleClicked(QTableWidgetItem* item);
    QString getTaskDescription(const QString& title, const QString& employeeName);
    void assignTaskAuto();


};


