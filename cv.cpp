#include "cv.h"
#include <iostream>


/*CV::WAIT
------------------------------
When wait is called, we first check if the thread who called it is actually holding a lock. If not, we throw a runtime error. 

If the thread is holding the lock, we set the owner of the lock (m.owner) to nullptr and empty the mutex's waiting queue, 
emplacing each thread onto the ready queue. We set the owner of the mutex (m.owner) to the first thread of the waiting queue, i.e. "the next in line".

Since we want to suspend the current thread, we add it onto the cv's waiting queue. 
We then check whether the ready queue is empty; if so, we can suspend the cpu and end the program. 

We then set the running thread to be the next element in the ready queue, and swap the thread that called wait and the new running thread. 

Next, we check whether or not the lock is free.
Again, if the ready queue is empty; if so, we can suspend the cpu and end the program. 
If the lock is not free, we add the current running thread onto the mutex's waiting queue.  
We also set the running thread to the front of the ready queue and swap to the new running thread. 
If the lock is free, we set the new owner of the lock to be the current running thread.
------------------------------*/



void cv::wait(mutex& m){

    cpu::self()->interrupt_disable();


    //begin releasing the lock! ------
    if(m.owner != cpu::self()->runningThread){
        cpu::self()->interrupt_enable();
        throw std::runtime_error("You are not the owner of the lock - CV::wait");
    }
    else{
        //step 1 : unlock mutex that is passed through by the thread
        m.owner = nullptr;
        //check if the waiting queue is empty for lock
        if(!m.waiting_queue.empty()){

            
            std::shared_ptr<threadContext> frontofWaitingQueue = m.waiting_queue.front();
            m.waiting_queue.pop();
            cpu::self()->ready_queue.emplace(frontofWaitingQueue);
            m.owner = frontofWaitingQueue;
        }
    }

    //step 2: add running thread to cv waiting queue
    cv_queue.emplace(cpu::self()->runningThread);

    //if the cpu is the running thread, nothing else to run in the ready queue
    // we can end with the cpu right away
    if(cpu::self()->ready_queue.empty()){
        cpu::self()->finished = true;
        cpu::self()->interrupt_enable_suspend();
    }

    //step 3 :  remove running thread from ready queue (sleeping)
    std::shared_ptr<threadContext> frontofReadyQueue = cpu::self()->ready_queue.front();
    cpu::self()->ready_queue.pop();
    cpu::self()->runningThread = frontofReadyQueue; //set the new running thread


    swapcontext(cv_queue.back()->ucontext_ptr, cpu::self()->runningThread->ucontext_ptr);

    //step 4: try to reaquire the lock
   
    //if lock is not free
    if(m.owner != nullptr && m.owner != cpu::self()->runningThread){
        m.waiting_queue.emplace(cpu::self()->runningThread);

        //if the cpu is the running thread, nothing else to run in the ready queue
        // we can end with the cpu right away
        if(cpu::self()->ready_queue.empty()){
            cpu::self()->finished = true;
            cpu::self()->interrupt_enable_suspend();
        }

        std::shared_ptr<threadContext> frontofReadyQueue = cpu::self()->ready_queue.front();
        cpu::self()->ready_queue.pop();


        cpu::self()->runningThread = frontofReadyQueue;

        swapcontext(m.waiting_queue.back()->ucontext_ptr, cpu::self()->runningThread->ucontext_ptr);
    }
    else{

        //if lock is free
        m.owner = cpu::self()->runningThread;
    }

    cpu::self()->interrupt_enable();

}



/*CV::SIGNAL
------------------------------
If the cv waiting queue is empty, this operation does nothing. 
If not, we emplace the thread at the front of the cv's waiting queue onto the back of the ready_queue, 
then pop the element off of the waiting queue.
------------------------------*/

void cv::signal(){

    cpu::self()->interrupt_disable();


    //check that the cv_queue is not empty, remove one element if not empty
    //if empty, do nothing and continue
    if(!cv_queue.empty()){

        //take the first of the waiting queue for the mutex
        //place it on the ready queue

        cpu::self()->ready_queue.emplace(cv_queue.front());

        cv_queue.pop();
    }

    cpu::self()->interrupt_enable();

}



/*CV::BROADCAST
------------------------------
If the cv waiting queue is empty, this operation does nothing. 
If not, we pop every item off of the cv's waiting queue and place it onto the cpu's ready queue.
------------------------------*/

void cv::broadcast(){

    cpu::self()->interrupt_disable();
    //pop off all elements waiting in the cv_queue
    
    //continue until the cv_queue is empty

    //if empty, do nothing and continue
    while(!cv_queue.empty()){
        //take the first of the waiting queue for the mutex
        //place it on the ready queue

        cpu::self()->ready_queue.emplace(cv_queue.front());

        cv_queue.pop();
    }
    cpu::self()->interrupt_enable();

}



cv::~cv(){

}
