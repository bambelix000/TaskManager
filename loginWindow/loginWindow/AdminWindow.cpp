#include "AdminWindow.h"
#include "ui_AdminWindow.h"




AdminWindow::AdminWindow(Admin* admin, QWidget* parent) :
    QMainWindow(parent), admin(admin), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle(admin->getWindowTitle());

    connect(ui->btnLogout, &QPushButton::clicked, this, &AdminWindow::logout);

    connect(ui->btnSaveTask, &QPushButton::clicked, this, &AdminWindow::onSaveTaskClicked);
    connect(ui->btnNewTask, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->pageNewTask);
        AdminWindow::loadUsersToAssign();
        });
    connect(ui->btnAddEmployee, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->pageAddEmployee);
        });
    connect(ui->btnSearchTasks, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->pageSearchTasks);
        loadAndDisplayTasks(ui->lineEditSearch->text());
        });

    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, [=](const QString& text) {
        loadAndDisplayTasks(text);
        });

    connect(ui->comboSort, &QComboBox::currentTextChanged, this, [=](const QString& option) {
        loadAndDisplayTasks(ui->lineEditSearch->text());
        });

    connect(ui->comboSelectEmployee, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminWindow::updateScheduleDays);

    connect(ui->btnAutoAssign, &QPushButton::clicked, this, &AdminWindow::assignTaskAuto);

    ui->tableWidgetTasks->verticalHeader()->setVisible(false);
    ui->tableWidgetTasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetTasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetTasks->setSelectionMode(QAbstractItemView::NoSelection);

    ui->dateEditTask->setMinimumDate(QDate::currentDate());
    ui->scheduleBars1->installEventFilter(this);
    ui->scheduleBars2->installEventFilter(this);
    ui->scheduleBars3->installEventFilter(this);

    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents); 
 

    ui->tableWidgetTasks->horizontalHeader()->setStretchLastSection(false); 

    QList<int> proportions = { 3, 2 }; 
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->editTaskName->setMaxLength(34);

    loadAndDisplayTasks("");
    loadUsersToAssign();
}

void AdminWindow::loadUsersToAssign()
{
    ui->comboAssignTo->clear();

    extern UserRepository* userRepository;
    if (!userRepository) return;

    for (const auto& pair : userRepository->usersMap) {
        int userId = pair.first;
        const auto& user = pair.second.user;
        if (!user) continue;
        if (user->getRole() == "employee") {
            QString fullName = QString::fromStdString(user->getFirstName() + " " + user->getLastName());
            ui->comboAssignTo->addItem(fullName, userId);
        }
    }
}
int ApriorityValue(const std::string& priority) {
    if (priority == "Wysoki") return 3;
    if (priority == "Średni") return 2;
    if (priority == "Niski")  return 1;
    return 0;
}

void AdminWindow::assignTaskAuto()
{
    double a = 1.0;
    double b = 0.7;
    double c = 0.5;

    extern UserRepository* userRepository;

    QString deadlineDateStr = ui->dateEditTask->text();
    QString priority = ui->comboPriority->currentText();

    QDate today = QDate::currentDate();
    QDate deadlineDate = QDate::fromString(deadlineDateStr, "d.MM.yyyy");

    int bestUserId = -1;
    double bestScore = -std::numeric_limits<double>::infinity();

    std::map<int, double> scores;
    for (const auto& pair : userRepository->usersMap) {
        int userId = pair.first;
        const auto& user = pair.second.user;
        if (!user) continue;
        if (user->getRole() != "employee") continue;

        const auto& schedule = pair.second.schedule;
        int totalAvailableHours = 0;
        for (QDate date = today; date <= deadlineDate; date = date.addDays(1)) {
            std::string dayStr = date.toString("yyyy-MM-dd").toStdString();
            std::vector<int> availableHours = schedule.getAvailability(dayStr);
            totalAvailableHours += static_cast<int>(availableHours.size());
        }

        int taskCountWithDeadline = 0;
        int prioritySum = 0;
        for (const auto& task : pair.second.tasks) {
            QString ori = QString::fromStdString(task.getDeadline());
            QString converted= QDate::fromString(ori, "yyyy-MM-dd").toString("d.MM.yyyy");
            if (converted == deadlineDateStr) {
                ++taskCountWithDeadline;
                prioritySum += ApriorityValue(task.getPriority());
            }
        }

        double score = a * totalAvailableHours
            - b * taskCountWithDeadline
            - c * prioritySum;
        scores[userId] = score;
        if (score > bestScore) {
            bestScore = score;
            bestUserId = userId;
        }
    }

        int numItems = ui->comboAssignTo->count();
        for (int i = 0; i < numItems; ++i) {
            QVariant data = ui->comboAssignTo->itemData(i);
            if (data.isValid() && data.toInt() == bestUserId) {
                ui->comboAssignTo->setCurrentIndex(i);
                break;
            }
        }
}

void AdminWindow::onSaveTaskClicked()
{
    extern UserRepository* userRepository;

    QString taskName = ui->editTaskName->text();
    QString taskDesc = ui->editTaskDesc->toPlainText();
    QDate taskDate = ui->dateEditTask->date();
    QString priority = ui->comboPriority->currentText();
    QVariant userIdVar = ui->comboAssignTo->currentData();

    int assignedUserId = userIdVar.toInt();

    if (taskName.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Nazwa zadania nie może być pusta.");
        return;
    }

    QStringList inputs = { taskName, taskDesc};
    for (const QString& input : inputs) {
        if (input.contains(';')) {
            QMessageBox::warning(this, "Błąd", "Żadne pole nie może zawierać średnika (;).");
            return;
        }
    }

    admin->newTask(
        assignedUserId,
        taskName.toStdString(),
        taskDesc.toStdString(),
        taskDate.toString(Qt::ISODate).toStdString(),
        priority.toStdString()
    );

    QMessageBox::information(this, "Sukces", "Zadanie zostało zapisane.");

    ui->editTaskName->clear();
    ui->editTaskDesc->clear();
    ui->dateEditTask->setDate(QDate::currentDate());
    ui->comboPriority->setCurrentIndex(0);
    ui->comboAssignTo->setCurrentIndex(0);
}

void AdminWindow::on_btnAddEmployeeConfirm_clicked() {
    extern UserRepository* userRepository;

    QString firstName = ui->lineEditFirstName->text();
    QString lastName = ui->lineEditLastName->text();
    QString email = ui->lineEditEmail->text();
    QString login = ui->lineEditLogin->text();
    QString password = ui->lineEditPassword->text();
    QString role = ui->comboBoxRole->currentText();

    if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty() ||
        login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę uzupełnić wszystkie pola");
        return;
    }

    QRegularExpression emailRegex(R"((^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))");
    QRegularExpressionMatch match = emailRegex.match(email);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Błąd", "Podaj poprawny adres e-mail.");
        return;
    }

    QStringList inputs = { firstName, lastName, email, login, password };
    for (const QString& input : inputs) {
        if (input.contains(';')) {
            QMessageBox::warning(this, "Błąd", "Żadne pole nie może zawierać średnika (;).");
            return;
        }
    }

	std::string hashedPassword = User::hashPassword(password.toStdString());

    std::string errorMsg;
    bool ok = admin->newUser(
        login.toStdString(),
        hashedPassword,
        role.toStdString(),
        firstName.toStdString(),
        lastName.toStdString(),
        email.toStdString(),
        &errorMsg
    );

    if (!ok) {
        QMessageBox::warning(this, "Błąd", QString::fromStdString(errorMsg));
        return;
    }

    QMessageBox::information(this, "Sukces", "Dodano nowego pracownika.");

    ui->lineEditFirstName->clear();
    ui->lineEditLastName->clear();
    ui->lineEditEmail->clear();
    ui->lineEditLogin->clear();
    ui->lineEditPassword->clear();
    ui->comboBoxRole->setCurrentIndex(0);
}



void AdminWindow::sortTaskList(std::vector<Task>& tasks, const QString& sortOption) {
    if (expandedRow != -1) {
        ui->tableWidgetTasks->removeRow(expandedRow);
        expandedRow = -1;
    }

    if (sortOption == "Deadline ↑") {
        std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
            return a.getDeadline() < b.getDeadline();
            });
    }
    else if (sortOption == "Deadline ↓") {
        std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
            return a.getDeadline() > b.getDeadline();
            });
    }
    else if (sortOption == "Priorytet ↓") {
        std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
            int pa = ApriorityValue(a.getPriority());
            int pb = ApriorityValue(b.getPriority());
            if (pa != pb)
                return pa > pb;
            return a.getDeadline() < b.getDeadline();
            });
    }
    else if (sortOption == "Priorytet ↑") {
        std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
            int pa = ApriorityValue(a.getPriority());
            int pb = ApriorityValue(b.getPriority());
            if (pa != pb)
                return pa < pb;
            return a.getDeadline() < b.getDeadline();
            });
    }

    std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
        return !a.getStatus() && b.getStatus();
        });
}

void AdminWindow::loadAndDisplayTasks(const QString& filterRegex)
{
    extern UserRepository* userRepository;
    QString sortOption = ui->comboSort->currentText();

    QString searchText = filterRegex; 
    std::string searchPattern = searchText.toStdString();
    std::regex searchRegex(searchPattern, std::regex_constants::icase);

    std::vector<const Task*> filteredTasks;
    std::vector<QString> employeeNames;

    for (const auto& pair : userRepository->usersMap) {
        const UserAggregate& aggregate = pair.second;
        if (!aggregate.user) continue;

        QString employeeName = QString::fromStdString(aggregate.user->getFirstName() + " " + aggregate.user->getLastName());
        for (const auto& task : aggregate.tasks) {
            std::string line = task.getTitle() + ";" +
                employeeName.toStdString() + ";" +
                task.getDeadline() + ";" +
                task.getCreatedDate() + ";" +
                task.getPriority();
            if (searchText.isEmpty() || std::regex_search(line, searchRegex)) {
                filteredTasks.push_back(&task);
                employeeNames.push_back(employeeName); 
            }
        }
    }

    std::vector<size_t> indices(filteredTasks.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        bool readyA = filteredTasks[a]->getStatus();
        bool readyB = filteredTasks[b]->getStatus();
        if (readyA != readyB) return !readyA;
        if (sortOption == "Deadline ↑") {
            QDate dA = QDate::fromString(QString::fromStdString(filteredTasks[a]->getDeadline()), "yyyy-MM-dd");
            QDate dB = QDate::fromString(QString::fromStdString(filteredTasks[b]->getDeadline()), "yyyy-MM-dd");
            return dA < dB;
        }
        else if (sortOption == "Deadline ↓") {
            QDate dA = QDate::fromString(QString::fromStdString(filteredTasks[a]->getDeadline()), "yyyy-MM-dd");
            QDate dB = QDate::fromString(QString::fromStdString(filteredTasks[b]->getDeadline()), "yyyy-MM-dd");
            return dA > dB;
        }
        else if (sortOption == "Priorytet ↓") {
            return ApriorityValue(filteredTasks[a]->getPriority())
            > ApriorityValue(filteredTasks[b]->getPriority());
        }
        else if (sortOption == "Priorytet ↑") {
            return ApriorityValue(filteredTasks[a]->getPriority())
                < ApriorityValue(filteredTasks[b]->getPriority());
        }
        return false;
        });

    QStringList headers = { "Tytuł", "Pracownik", "Deadline", "Data dodania", "Priorytet", "Gotowość" };
    ui->tableWidgetTasks->setColumnCount(headers.size());
    ui->tableWidgetTasks->setHorizontalHeaderLabels(headers);
    ui->tableWidgetTasks->clearContents();
    ui->tableWidgetTasks->setRowCount(static_cast<int>(filteredTasks.size()));

    for (int i = 0; i < static_cast<int>(filteredTasks.size()); ++i) {
        const Task* task = filteredTasks[indices[i]];
        QString employeeName = employeeNames[indices[i]];
        bool isReady = task->getStatus();

        auto* itemTitle = new QTableWidgetItem(QString::fromStdString(task->getTitle()));
        auto* itemEmployee = new QTableWidgetItem(employeeName);
        auto* itemDeadline = new QTableWidgetItem(QString::fromStdString(task->getDeadline()));
        auto* itemAssigned = new QTableWidgetItem(QString::fromStdString(task->getCreatedDate()));
        auto* itemPriority = new QTableWidgetItem(QString::fromStdString(task->getPriority()));
        QString readyStr = isReady ? "Tak" : "Nie";
        auto* itemReady = new QTableWidgetItem(readyStr);

        if (!isReady) {
            QFont boldFont;
            boldFont.setBold(true);
            itemTitle->setFont(boldFont);
            itemEmployee->setFont(boldFont);
            itemDeadline->setFont(boldFont);
            itemAssigned->setFont(boldFont);
            itemPriority->setFont(boldFont);
            itemReady->setFont(boldFont);

            itemTitle->setBackground(QColor("#e8d49e"));
            itemEmployee->setBackground(QColor("#e8d49e"));
            itemDeadline->setBackground(QColor("#e8d49e"));
            itemAssigned->setBackground(QColor("#e8d49e"));
            itemPriority->setBackground(QColor("#e8d49e"));
            itemReady->setBackground(QColor("#e8d49e"));
        }
        else {
            QBrush grayBrush(Qt::gray);
            itemTitle->setForeground(grayBrush);
            itemEmployee->setForeground(grayBrush);
            itemDeadline->setForeground(grayBrush);
            itemAssigned->setForeground(grayBrush);
            itemPriority->setForeground(grayBrush);
            itemReady->setForeground(grayBrush);
        }

        ui->tableWidgetTasks->setItem(i, 0, itemTitle);
        ui->tableWidgetTasks->setItem(i, 1, itemEmployee);
        ui->tableWidgetTasks->setItem(i, 2, itemDeadline);
        ui->tableWidgetTasks->setItem(i, 3, itemAssigned);
        ui->tableWidgetTasks->setItem(i, 4, itemPriority);
        ui->tableWidgetTasks->setItem(i, 5, itemReady);
    }
}

void AdminWindow::on_tableWidgetTasks_itemDoubleClicked(QTableWidgetItem* item) {
    int row = item->row();


    if (expandedRow == row + 1) {
        ui->tableWidgetTasks->removeRow(expandedRow);
        expandedRow = -1;
        return;
    }

    if (expandedRow != -1) {
        ui->tableWidgetTasks->removeRow(expandedRow);
        if (row > expandedRow) row--; 
    }

    QString title = ui->tableWidgetTasks->item(row, 0)->text();
    QString employeeName = ui->tableWidgetTasks->item(row, 1)->text();
    QString description = getTaskDescription(title, employeeName); 

    ui->tableWidgetTasks->insertRow(row + 1);
    QTableWidgetItem* descItem = new QTableWidgetItem(description);
    descItem->setFlags(Qt::ItemIsEnabled);
    descItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop); 
    ui->tableWidgetTasks->setItem(row + 1, 0, descItem);
    ui->tableWidgetTasks->setSpan(row + 1, 0, 1, ui->tableWidgetTasks->columnCount());

    QFontMetrics fm(descItem->font());
    int width = 0;
    for (int col = 0; col < ui->tableWidgetTasks->columnCount(); ++col)
        width += ui->tableWidgetTasks->columnWidth(col);

    QRect r = fm.boundingRect(QRect(0, 0, width, 10000), Qt::TextWordWrap, description);
    ui->tableWidgetTasks->setRowHeight(row + 1, r.height() + 10);

    expandedRow = row + 1;
}

QString AdminWindow::getTaskDescription(const QString& title, const QString& employeeName)
{
    extern UserRepository* userRepository;
    for (const auto& pair : userRepository->usersMap) {
        const UserAggregate& aggregate = pair.second;
        QString ename = QString::fromStdString(aggregate.user->getFirstName() + " " + aggregate.user->getLastName());
        if (ename != employeeName) continue;
        for (const auto& task : aggregate.tasks) {
            if (QString::fromStdString(task.getTitle()) == title) {
                return QString::fromStdString(task.getDescription());
            }
        }
    }
    return "Brak opisu";
}

void AdminWindow::closeEvent(QCloseEvent* event){
    userRepository->saveAll();
    QMainWindow::closeEvent(event);
}

bool AdminWindow::eventFilter(QObject* obj, QEvent* event) {
    if ((obj == ui->scheduleBars1 || obj == ui->scheduleBars2 || obj == ui->scheduleBars3)
        && event->type() == QEvent::Resize) {
        updateScheduleDays();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void AdminWindow::on_btnPrevDays_clicked() {
    if (scheduleOffset >= -3) {
        scheduleOffset -= 3;
        updateScheduleDays();
    }
}

void AdminWindow::on_btnNextDays_clicked() {
    scheduleOffset += 3;
    updateScheduleDays();
}

void AdminWindow::updateScheduleDays() {
    std::vector<QGroupBox*> groups = { ui->groupDay1, ui->groupDay2, ui->groupDay3 };
    std::vector<QWidget*> bars = { ui->scheduleBars1, ui->scheduleBars2, ui->scheduleBars3 };

    QVariant userIdVar = ui->comboSelectEmployee->currentData();
    if (!userIdVar.isValid()) return;
    int userId = userIdVar.toInt();
    extern UserRepository* userRepository;
    auto agg = userRepository->getUserAggregate(userId);
    if (!agg) return;
    Schedule schedule = agg->schedule;

    for (int i = 0; i < 3; ++i) {
        QDate date = QDate::currentDate().addDays(scheduleOffset + i);
        QString dayStr = date.toString("yyyy-MM-dd");
        groups[i]->setTitle(date.toString("d.MM.yyyy"));

        std::vector<int> hours = schedule.getAvailability(dayStr.toStdString());

        QPixmap pixmap(bars[i]->size());
        pixmap.fill(Qt::white);
        QPainter painter(&pixmap);

        int marginLeft = 28; 
        int marginTop = 10;
        int marginRight = 10;
        int marginBottom = 10;
        int hourCount = 24;
        int barHeight = pixmap.height() / hourCount;
        int barWidth = pixmap.width(); 

        QFont font = painter.font();
        font.setPointSize(std::max(8, barHeight / 2));
        painter.setFont(font);

        for (int h = 0; h < hourCount; ++h) {
            bool available = std::find(hours.begin(), hours.end(), h) != hours.end();
            int y = h * barHeight;
            QRect barRect(0, y, barWidth, barHeight);

            painter.setBrush(available ? Qt::green : Qt::lightGray);
            painter.setPen(Qt::black);
            painter.drawRect(barRect);
            painter.setPen(Qt::black);
            painter.drawText(6, y + barHeight / 2 + 4, QString::number(h));
        }

        bars[i]->setAutoFillBackground(true);
        QPalette pal = bars[i]->palette();
        pal.setBrush(QPalette::Window, pixmap);
        bars[i]->setPalette(pal);
        bars[i]->update();
    }
}

void AdminWindow::on_btnSchedule_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageEmployeeSchedule);

    ui->comboSelectEmployee->clear();
    extern UserRepository* userRepository;
    for (const auto& pair : userRepository->usersMap) {
        const auto& user = pair.second.user;
        if (user && user->getRole() == "employee") {
            int userId = pair.first;
            QString fullName = QString::fromStdString(user->getFirstName() + " " + user->getLastName());
            ui->comboSelectEmployee->addItem(fullName, userId);
        }
    }
    if (ui->comboSelectEmployee->count() > 0)
        ui->comboSelectEmployee->setCurrentIndex(0);
}

void AdminWindow::logout()
{
    this->close();
    loginWindow* loginWin = new loginWindow();
    loginWin->show();
}

AdminWindow::~AdminWindow() {
    delete ui;
}


