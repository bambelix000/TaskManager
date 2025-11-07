#include "loginWindow.h"
#include "ui_loginWindow.h"


UserRepository g_userRepo;
UserRepository* userRepository = &g_userRepo;

loginWindow::loginWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::loginWindow)
{
    ui->setupUi(this);
    g_userRepo.loadAll();
}

loginWindow::~loginWindow() {
    delete ui;
}

void loginWindow::on_pushButton_clicked() {
    QString login = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();

    int userId = g_userRepo.findUserIdByLogin(login.toStdString());
    if (userId == -1) {
        QMessageBox::warning(this, "B³¹d", "Niepoprawny login lub has³o.");
        return;
    }

    UserAggregate* agg = g_userRepo.getUserAggregate(userId);
    if (!agg || !agg->user) {
        QMessageBox::warning(this, "B³¹d", "Niepoprawny login lub has³o.");
        return;
    }

    std::string hashedInput = User::hashPassword(password.toStdString());
    if (agg->user->getPassword() != hashedInput) {
        QMessageBox::warning(this, "B³¹d", "Niepoprawny login lub has³o.");
        return;
    }

    if (agg->user->getRole() == "admin") {
        AdminWindow* adminWin = new AdminWindow(static_cast<Admin*>(agg->user.get()));
        adminWin->show();
        this->close();
    }
    else if (agg->user->getRole() == "employee") {
        EmployeeWindow* empWin = new EmployeeWindow(std::static_pointer_cast<Employee>(agg->user));
        empWin->show();
        this->close();
    }
    else {
        QMessageBox::warning(this, "B³¹d", "Nieobs³ugiwana rola u¿ytkownika.");
    }
}

void loginWindow::closeEvent(QCloseEvent* event)
{
    if (userRepository) {
        userRepository->saveAll();
    }
    QMainWindow::closeEvent(event);
}
