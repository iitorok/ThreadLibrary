#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"


using std::cout;
using std::endl;

mutex mutex1;
cv cvping;
cv cvpong;
cv cvname;
int hits = 0;
thread* threadPtr = nullptr;
thread* threadPtr2 = nullptr;
thread* threadPtr3 = nullptr;
thread* threadPtr4 = nullptr;
thread* threadPtr5 = nullptr;
thread* threadPtr6 = nullptr;
thread* threadPtr7 = nullptr;
thread* threadPtr8 = nullptr;
thread* threadPtr9 = nullptr;

void name2(uintptr_t arg)
{
    mutex1.lock();
    thread::yield();
    if(threadPtr == nullptr){
      thread::yield();
      threadPtr5 = new thread(name2, arg);
      thread::yield();
      cvname.signal();
      thread::yield();
    }
    cout << "name 2" << endl;
    thread::yield();
    cvname.wait(mutex1);
    thread::yield();
    if (hits == 0){
        thread::yield();
        cout << "waking people up for fun" << endl;
        thread::yield();
        cvname.signal();
        thread::yield();
        cvping.signal();
        thread::yield();
        cvpong.signal();
    }
    thread::yield();
    mutex1.unlock();
}


void name(uintptr_t arg)
{
    mutex1.lock();
    thread::yield();
    cout << "grabbed lock" << endl;
    thread::yield();
    thread t1(name2, arg);
    thread::yield();
    cvname.wait(mutex1);
    thread::yield();
    threadPtr = new thread (name2, arg);
    thread::yield();
    cvpong.signal();
    thread::yield();
    cvname.wait(mutex1);
    thread::yield();
    hits = 0;
    thread::yield();
    cvping.broadcast();
    thread::yield();
    if(hits == 0){
      thread::yield();
      cout << "wow new round of pingpong" << endl;
      thread::yield();
      if(threadPtr3 != nullptr){
        thread::yield();
        threadPtr3->join();
        thread::yield();
      }
      thread::yield();
      if(threadPtr6 != nullptr){
        thread::yield();
        threadPtr6 = new thread(name, arg);
        thread::yield();
      }
      thread::yield();
      cvname.wait(mutex1);
      thread::yield();
    }
    thread::yield();
    if (threadPtr4 != nullptr){
        thread::yield();
        threadPtr4->join();
        thread::yield();
    }
    thread::yield();
    mutex1.unlock();
}

void pong(uintptr_t arg)
{
    thread::yield();
    while ( hits != 6){
        thread::yield();
        mutex1.lock();
        thread::yield();
        cout << "pong" << endl;
        thread::yield();
        cvping.signal();
        thread::yield();
        cvpong.wait(mutex1);
        thread::yield();
        hits++;
        thread::yield();
        mutex1.unlock();
        thread::yield();
    }
    thread::yield();
    mutex1.lock();
    thread::yield();
    cout << "pong won" << endl;
    thread::yield();
    cvname.broadcast();
    thread::yield();
    cvpong.wait(mutex1);
    thread::yield();
    if (hits == 0){
        thread::yield();
        threadPtr4 = new thread (pong, arg);
        thread::yield();
    }
    thread::yield();
    mutex1.unlock();
    thread::yield();
}

void ping(uintptr_t arg)
{
    thread t1(pong, arg);
    while (hits != 5){
        thread::yield();
        mutex1.lock();
        thread::yield();
        cout << "ping " << endl;
        thread::yield();
        cvpong.signal();
        thread::yield();
        cvping.wait(mutex1);
        thread::yield();
        mutex1.unlock();
        thread::yield();
    }
    thread::yield();
    mutex1.lock();
    thread::yield();
    cout << "ping won" << endl;
    thread::yield();
    thread t2(name, arg);
    thread::yield();
    cvname.broadcast();
    thread::yield();
    cvping.wait(mutex1);
    thread::yield();
    if (hits == 0){
        thread::yield();
        threadPtr3 = new thread (ping, arg);
        thread::yield();
    }
    thread::yield();
    cvping.wait(mutex1);
    thread::yield();
    if ( threadPtr7 != nullptr ){
        thread::yield();
        threadPtr7 = new thread (name, arg);
        thread::yield();
    }
    thread::yield();
    mutex1.unlock();
    thread::yield();
}



int main()
{
    cpu::boot(1, ping, 0, false, false, 0);
}