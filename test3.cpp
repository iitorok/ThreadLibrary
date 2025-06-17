#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

bool third_done = false;
int counter = 0;

cv c;
mutex m;

void third(uintptr_t arg)
{
    m.lock();
    cout<<"reached the third"<<endl;
    ++counter;

    if(counter == 2){
        third_done = true;
        c.signal();
    }

    m.unlock();
}

void second(uintptr_t arg)
{
    m.lock();
    thread t(third, reinterpret_cast<uintptr_t>("third"));
    m.unlock();
}


void first(uintptr_t arg)
{
    m.lock();
    thread t1 (second, reinterpret_cast<uintptr_t>("second"));
    
    thread t2 (third, reinterpret_cast<uintptr_t>("third"));

    
    while(third_done == false){
        c.wait(m);
    }

    t1.join();

    m.unlock();

}

int main()
{
    cpu::boot(1, first, 0, false, false, 3);
}