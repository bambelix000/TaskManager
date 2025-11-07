#pragma once
#include <string>
#include <sstream>

class Task {
public:
    Task(const std::string& title,
         const std::string& description,
         const std::string& deadline,
         const std::string& assignedTo,
         const std::string& createdDate,
         bool status,
         const std::string& priority);

    // gettery
    std::string getTitle() const;
    std::string getDescription() const;
    std::string getDeadline() const;
    bool getStatus() const;
    std::string getAssignedTo() const;
    std::string getCreatedDate() const;
    std::string getPriority() const;

    // setter
    void setStatus(bool ready);

    static Task fromString(const std::string& line);
    std::string toString() const;

private:
    std::string title;
    std::string description;
    std::string deadline;
    std::string assignedTo;
    std::string createdDate;
    bool status;
    std::string priority;
};
