#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"


using std::cout;
using std::endl;

mutex mutex1;
cv cv1;
thread* threadPtr = nullptr;
//int num = 0;

void notchild(uintptr_t arg)
{
    mutex1.lock();
    cout << "I am: NOT child = "<< arg <<" thread" << endl;
    mutex1.unlock();
}

void child(uintptr_t arg)
{
    mutex1.lock();
    threadPtr->join();
    cout << "I am: child = "<< arg <<" thread" << endl;
    mutex1.unlock();
}

void parent(uintptr_t arg)
{
	cv1.signal();
    cv1.broadcast();
    cout << "I am: parent" << endl;
    thread t1 (child, arg);
    threadPtr = new thread(notchild, arg);
    thread::yield();
    delete threadPtr;
    threadPtr = nullptr;
    t1.join();
    cout << "I am: parent" << endl;
}

int main()
{
    cpu::boot(1, parent, 0, false, false, 0);
}