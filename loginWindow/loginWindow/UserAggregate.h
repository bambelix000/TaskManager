#pragma once
#include "User.h"
#include "Task.h"
#include "Schedule.h"
#include <vector>
#include <memory>

struct UserAggregate {
    std::shared_ptr<User> user;     
    std::vector<Task> tasks;          
    Schedule schedule;            

    UserAggregate() : user(nullptr), tasks(), schedule(0) {}
    UserAggregate(std::shared_ptr<User> u, const std::vector<Task>& t, const Schedule& s)
        : user(u), tasks(t), schedule(s) {
    }
};

