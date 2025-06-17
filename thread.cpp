#include "thread.h"
#include "ucontext.h"
//#include "cpu.cpp"
//#include "cpu.h"
#include <iostream>


thread::thread(thread_startfunc_t func, uintptr_t arg){

    //pointer to a thread context
    //this will call the constructor in the threadContext class
    context_ptr =  std::make_shared<threadContext>(func,arg);

    //4. Add TCB to ready queue *DON'T FORGET!!!
    cpu::self()->ready_queue.emplace(context_ptr);


}

void thread::yield(){
    cpu::self()->interrupt_disable();

    //check if the ready queue is empty
    if(cpu::self()->ready_queue.empty()){
        cpu::self()->interrupt_enable();
        return;
    }

    //GOAL: Switch CPU with Parent


    std::shared_ptr<threadContext> frontofQueue = cpu::self()->ready_queue.front();
    cpu::self()->ready_queue.pop();
    cpu::self()->ready_queue.emplace(cpu::self()->runningThread);


    //set runningThread to the front of the queue
    cpu::self()->runningThread = frontofQueue;

    //swap - saves old running thread conetxt with the new running thread context from front of queue
    swapcontext(cpu::self()->ready_queue.back()->ucontext_ptr, cpu::self()->runningThread->ucontext_ptr);

    cpu::self()->interrupt_enable();

}

void thread::join() {


    cpu::self()->interrupt_disable();
    //check if this thread that is being joined on still running
    if (context_ptr->threadQueue.front() == nullptr && context_ptr->threadQueue.size() == 1){
        cpu::self()->interrupt_enable();
        return;
    }

    //if thread1 is running
    //and we call thread2.join()
    //which means we put thread1 on thread2's waiting list to finish
    context_ptr->threadQueue.emplace(cpu::self()->runningThread);

    //swap context with the next avaliable thread

    //if someone tries to join with themselves
    if(cpu::self()->ready_queue.empty()){
        cpu::self()->finished = true;
        cpu::self()->interrupt_enable_suspend();
    }


    std::shared_ptr<threadContext> frontofReadyQueue = cpu::self()->ready_queue.front();
    cpu::self()->ready_queue.pop();

    //set runningThread to the front of the queue
    cpu::self()->runningThread = frontofReadyQueue;

    //swap - saves old running thread conetxt with the new running thread context from front of the ready queue
    swapcontext(context_ptr->threadQueue.back()->ucontext_ptr, cpu::self()->runningThread->ucontext_ptr);


    cpu::self()->interrupt_enable();


}

thread::~thread() {
    //std::cout<< "Called destructor for thread\n";


}