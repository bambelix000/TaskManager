#pragma once

#include <QMainWindow>
#include "User.h"
#include "AdminWindow.h"
#include "EmployeeWindow.h"
#include "UserRepository.h"
#include <QMessageBox>
#include <QString>
#include <memory>
#include <QCloseEvent>

namespace Ui {
    class loginWindow;
}

class loginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget* parent = nullptr);
    ~loginWindow();

private slots:
    // Slot obs³uguj¹cy klikniêcie przycisku logowania.
    void on_pushButton_clicked();

private:
    Ui::loginWindow* ui;

protected:
    // Obs³uguje zdarzenie zamkniêcia okna logowania.
    void closeEvent(QCloseEvent* event) override;

};


