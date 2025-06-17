#include "mutex.h"
#include <iostream>


mutex::mutex(){

}

void mutex::lock(){

    //if thread is waiting on a lock, should not be in ready queue!
    //DISABLE INTERRUPTS
    cpu::self()->interrupt_disable(); 
    //"Even trivial deadlocks are legal". If a thread has the lock, it should still be able to lock itself out.
    /*if(owner == cpu::self()->runningThread){
        throw std::runtime_error("You already have this lock");
    }*/
    //else{
        //check if there is a current holder
        if(owner != nullptr){


            waiting_queue.emplace(cpu::self()->runningThread);


            //if the cpu is the running thread, nothing else to run in the ready queue
            // we can end with the cpu right away
            if(cpu::self()->ready_queue.empty()){
                cpu::self()->finished = true;
                cpu::self()->interrupt_enable_suspend();
            }


            std::shared_ptr<threadContext> frontofReadyQueue = cpu::self()->ready_queue.front();
            cpu::self()->ready_queue.pop();
            cpu::self()->runningThread = frontofReadyQueue;

            swapcontext(waiting_queue.back()->ucontext_ptr,cpu::self()->runningThread->ucontext_ptr);
        }
        else{


            owner = cpu::self()->runningThread;
        }
    //}

    cpu::self()->interrupt_enable();
}



void mutex::unlock(){

    cpu::self()->interrupt_disable();
    //std::cout << "owner of lock: " << owner << std::endl;
    //std::cout << "running thread: " << cpu::self()->runningThread << std::endl;


    if(owner != cpu::self()->runningThread){
        cpu::self()->interrupt_enable();
        throw std::runtime_error("You are not the owner of this lock -- mutex::unlock");
    }
    else{
        owner = nullptr;
        if(!waiting_queue.empty()){


            std::shared_ptr<threadContext> frontofWaitingQueue = waiting_queue.front();
            waiting_queue.pop();
            cpu::self()->ready_queue.emplace(frontofWaitingQueue);
            owner = frontofWaitingQueue;
        }
    }
    cpu::self()->interrupt_enable();
}


mutex::~mutex(){

}

