#include "EmployeeWindow.h"
#include "ui_EmployeeWindow.h"
#include "loginWindow.h"
#include "UserRepository.h" 
#include "Task.h"  


EmployeeWindow::EmployeeWindow(std::shared_ptr<Employee> employee, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::EmployeeWindow), employee(employee) {
    ui->setupUi(this);

    setWindowTitle(employee->getWindowTitle());
    ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetTasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetTasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetTasks->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidgetTasks->verticalHeader()->setVisible(false);

    connect(ui->btnTasks, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->pageTasks);
        });
    connect(ui->btnSchedule, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->pageSchedule);
        });
    connect(ui->btnPrevDays, &QPushButton::clicked, this, &EmployeeWindow::on_buttonSaveSchedule_clicked);
    connect(ui->btnNextDays, &QPushButton::clicked, this, &EmployeeWindow::on_buttonSaveSchedule_clicked);
    connect(ui->btnNextDays, &QPushButton::clicked, this, &EmployeeWindow::on_btnNextDays_clicked);
    connect(ui->btnPrevDays, &QPushButton::clicked, this, &EmployeeWindow::on_btnPrevDays_clicked);
    connect(ui->btnLogout, &QPushButton::clicked, this, &EmployeeWindow::logout);
    connect(ui->comboSort, &QComboBox::currentTextChanged, this, &EmployeeWindow::refreshTasks);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &EmployeeWindow::refreshTasks);


    refreshTasks();
    currentStartDate = QDate::currentDate();
    updateScheduleDays();
    refreshAvailability();
}

EmployeeWindow::~EmployeeWindow() {
    delete ui;
}

void EmployeeWindow::refreshAvailability() {
    extern UserRepository* userRepository;
    int userId = employee->getId();
    Schedule* schedule = userRepository->getUserSchedule(userId);
    if (!schedule) return;

    struct DayWidgets {
        QGroupBox* group;
        QTimeEdit* timeFrom;
        QTimeEdit* timeTo;
    };
    std::vector<DayWidgets> days = {
        {ui->groupDay1, ui->timeFrom1, ui->timeTo1},
        {ui->groupDay2, ui->timeFrom2, ui->timeTo2},
        {ui->groupDay3, ui->timeFrom3, ui->timeTo3}
    };

    for (const auto& dayWidgets : days) {
        if (!dayWidgets.group || !dayWidgets.timeFrom || !dayWidgets.timeTo) continue;

        QString dayTitle = dayWidgets.group->title(); 
        QDate qdate = QDate::fromString(dayTitle, "d.MM.yyyy");
        if (!qdate.isValid()) continue;
        QString dayKey = qdate.toString("yyyy-MM-dd");


        auto map = schedule->getAllAvailability();
        auto it = map.find(dayKey.toStdString());
        if (it != map.end() && !it->second.empty()) {
            int from = it->second.front();
            int to = it->second.back();
            dayWidgets.timeFrom->setTime(QTime(from, 0));
            dayWidgets.timeTo->setTime(QTime(to, 0));
        }
        else {
            dayWidgets.timeFrom->setTime(QTime(0, 0));
            dayWidgets.timeTo->setTime(QTime(0, 0));
        }
    }
}


void EmployeeWindow::on_buttonSaveSchedule_clicked() {
    extern UserRepository* userRepository;
    int userId = employee->getId();

    struct DayWidgets {
        QGroupBox* group;
        QTimeEdit* timeFrom;
        QTimeEdit* timeTo;
    };
    std::vector<DayWidgets> days = {
        {ui->groupDay1, ui->timeFrom1, ui->timeTo1},
        {ui->groupDay2, ui->timeFrom2, ui->timeTo2},
        {ui->groupDay3, ui->timeFrom3, ui->timeTo3}
    };

    for (const auto& dayWidgets : days) {
        if (!dayWidgets.group || !dayWidgets.timeFrom || !dayWidgets.timeTo) continue;

        QString dayTitle = dayWidgets.group->title();
        QDate qdate = QDate::fromString(dayTitle, "d.MM.yyyy");
        if (!qdate.isValid()) continue;
        QString dayKey = qdate.toString("yyyy-MM-dd");

        QTime tFrom = dayWidgets.timeFrom->time();
        QTime tTo = dayWidgets.timeTo->time();
        if (tFrom >= tTo) continue;

        std::vector<int> hours;
        for (int h = tFrom.hour(); h <= tTo.hour(); ++h) {
            hours.push_back(h);
        }

        Schedule* schedule = userRepository->getUserSchedule(userId);
        if (schedule) {
            schedule->setAvailability(dayKey.toStdString(), hours);
        }
    }

    refreshAvailability();
}

void EmployeeWindow::updateScheduleDays() {
    std::vector<QGroupBox*> groups = { ui->groupDay1, ui->groupDay2, ui->groupDay3 };
    for (int i = 0; i < 3; ++i) {
        QDate date = QDate::currentDate().addDays(scheduleOffset + i);
        groups[i]->setTitle(date.toString("d.MM.yyyy"));
    }
}

void EmployeeWindow::on_btnPrevDays_clicked() {
    if (scheduleOffset >= 3) {
        scheduleOffset -= 3;
        updateScheduleDays();
        refreshAvailability();
    }
}

void EmployeeWindow::on_btnNextDays_clicked() {
    scheduleOffset += 3;
    updateScheduleDays();
    refreshAvailability();
}


void EmployeeWindow::onTaskCheckBoxStateChanged(int state) {
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
    if (!checkBox) return;
    QString timestamp = checkBox->property("timestamp").toString();
    bool newStatus = (state == Qt::Checked);

    extern UserRepository* userRepository;
    int userId = employee->getId();
    std::vector<Task>* tasks = userRepository->getUserTasks(userId);
    if (!tasks) return;

    for (auto& task : *tasks) {
        if (QString::fromStdString(task.getCreatedDate()) == timestamp) {
            task.setStatus(newStatus);
            break;
        }
    }
    refreshTasks();
}


    int priorityValue(const std::string& priority) {
        if (priority == "Wysoki") return 3;
        if (priority == "Średni") return 2;
        if (priority == "Niski")  return 1;
        return 0;
    }


void EmployeeWindow::sortTasks(std::vector<Task>& tasks, const QString& sortOption) {
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
            int pa = priorityValue(a.getPriority());
            int pb = priorityValue(b.getPriority());
            if (pa != pb)
                return pa > pb;
            return a.getDeadline() < b.getDeadline();
            });
    }
    else if (sortOption == "Priorytet ↑") {
        std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
            int pa = priorityValue(a.getPriority());
            int pb = priorityValue(b.getPriority());
            if (pa != pb)
                return pa < pb;
            return a.getDeadline() < b.getDeadline();
            });
    }

    std::ranges::stable_sort(tasks, [](const Task& a, const Task& b) {
        return !a.getStatus() && b.getStatus();
        });
}

void EmployeeWindow::refreshTasks() {
    extern UserRepository* userRepository;
    std::vector<Task>* tasks = userRepository->getUserTasks(employee->getId());
    if (!tasks) return;

    QString sortOption = ui->comboSort->currentText();

    QString searchText = ui->lineEditSearch->text();
    std::string searchPattern = searchText.toStdString();
    std::regex searchRegex(searchPattern, std::regex_constants::icase);

    std::vector<Task> filteredTasks;
    for (const auto& task : *tasks) {
        if (std::regex_search(task.getTitle(), searchRegex)) {
            filteredTasks.push_back(task);
        }
    }

    sortTasks(filteredTasks, sortOption);

    ui->tableWidgetTasks->setRowCount(static_cast<int>(filteredTasks.size()));

    for (int i = 0; i < filteredTasks.size(); ++i) {
        const Task& task = filteredTasks[i];
        bool isReady = task.getStatus();

        auto* itemTitle = new QTableWidgetItem(QString::fromStdString(task.getTitle()));
        auto* itemDeadline = new QTableWidgetItem(QString::fromStdString(task.getDeadline()));
        auto* itemCreated = new QTableWidgetItem(QString::fromStdString(task.getCreatedDate()));
        auto* itemPriority = new QTableWidgetItem(QString::fromStdString(task.getPriority()));


        if (!isReady) {
            QFont boldFont;
            boldFont.setBold(true);
            itemTitle->setFont(boldFont);
            itemDeadline->setFont(boldFont);
            itemCreated->setFont(boldFont);
            itemPriority->setFont(boldFont);


            itemTitle->setBackground(QColor("#e8d49e"));
            itemDeadline->setBackground(QColor("#e8d49e"));
            itemCreated->setBackground(QColor("#e8d49e"));
            itemPriority->setBackground(QColor("#e8d49e"));
        }
        else {
            QBrush grayBrush(Qt::gray);
            itemTitle->setForeground(grayBrush);
            itemDeadline->setForeground(grayBrush);
            itemCreated->setForeground(grayBrush);
            itemPriority->setForeground(grayBrush);
        }

        ui->tableWidgetTasks->setItem(i, 0, itemTitle);
        ui->tableWidgetTasks->setItem(i, 1, itemDeadline);
        ui->tableWidgetTasks->setItem(i, 2, itemCreated);
        ui->tableWidgetTasks->setItem(i, 3, itemPriority);

        QWidget* checkBoxWidget = new QWidget();
        QCheckBox* checkBox = new QCheckBox(checkBoxWidget);
        checkBox->setChecked(isReady);
        checkBox->setEnabled(true);
        checkBox->setProperty("timestamp", QString::fromStdString(task.getCreatedDate()));
        connect(checkBox, &QCheckBox::checkStateChanged, this, &EmployeeWindow::onTaskCheckBoxStateChanged);

        QHBoxLayout* layout = new QHBoxLayout(checkBoxWidget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        checkBoxWidget->setLayout(layout);
        ui->tableWidgetTasks->setCellWidget(i, 4, checkBoxWidget);

        ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // Tytuł
        ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Deadline
        ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Data dodania
        ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Data dodania
        ui->tableWidgetTasks->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Data dodania

    }
}

void EmployeeWindow::logout() {
    this->close();
    loginWindow* loginWin = new loginWindow();
    loginWin->show();
}

void EmployeeWindow::closeEvent(QCloseEvent* event)
{
    userRepository->saveAll();
    QMainWindow::closeEvent(event);
}

void EmployeeWindow::on_tableWidgetTasks_itemDoubleClicked(QTableWidgetItem* item) {
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
    QString description = getTaskDescriptionForEmployee(title);

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

QString EmployeeWindow::getTaskDescriptionForEmployee(const QString& title)
{
    extern UserRepository* userRepository;
    for (const auto& pair : userRepository->usersMap) {
        const UserAggregate& aggregate = pair.second;
        QString ename = QString::fromStdString(aggregate.user->getFirstName() + " " + aggregate.user->getLastName());
        for (const auto& task : aggregate.tasks) {
            if (QString::fromStdString(task.getTitle()) == title) {
                return QString::fromStdString(task.getDescription());
            }
        }
    }
    return "Brak opisu";
}