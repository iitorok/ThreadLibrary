#include "cpu.h"
#include "thread.h"
#include <queue>
#include <iostream>


void interrupt(){

    thread::yield();

}

//CPU CONSTRUCTOR: "CAUSES THE CPU TO RUN THREADS AS THEY BECOME AVAILABLE."
//"CREATES A THREAD THAT EXECUTES FUNC(ARG)"
cpu::cpu(thread_startfunc_t func, uintptr_t arg){
    interrupt_vector_table[0] = &interrupt;

    //get the current context of the thread
    //getcontext copies the context of the current thread to the context pointed to by ucp:

    ucontext_t* cpu_arg = new ucontext_t;
    std::shared_ptr<threadContext> ucp = std::make_shared<threadContext>(cpu_arg);

    //set running thread to ucp
    cpu::self()->runningThread = ucp;


    //create the parent thread
    thread(func,arg);

    std::shared_ptr<threadContext> frontofQueue = cpu::self()->ready_queue.front();
    cpu::self()->ready_queue.pop();
    cpu::self()->runningThread = frontofQueue;

    setcontext(cpu::self()->runningThread->ucontext_ptr);

}









