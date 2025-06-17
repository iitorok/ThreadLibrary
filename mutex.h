#include "cpu.h"

/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

class mutex {
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    mutex(mutex&&);
    mutex& operator=(mutex&&);


    std::queue<std::shared_ptr<threadContext>> waiting_queue;
    std::shared_ptr<threadContext> owner = nullptr;

    //track the owner of the thread


    //bool status_free = true;
};