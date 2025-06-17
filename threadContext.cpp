#include "threadContext.h"
#include <iostream>
#include "cpu.h"
#include "ucontext.h"



threadContext::threadContext(ucontext_t* arg){
    ucontext_ptr = arg;
    delete[] charPtr;
    charPtr = nullptr;
    getcontext(ucontext_ptr);
}

threadContext::threadContext(thread_startfunc_t func, uintptr_t arg){
    //1. Allocate TCB (Thread Control Block) - //2. Allocate stack
    ucontext_ptr->uc_stack.ss_sp = static_cast<char*>(charPtr);
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = nullptr;

    //2. Initialize context in TCB - makecontext() initializes context and stack
    makecontext(ucontext_ptr, reinterpret_cast<void (*)()>(handler), 3, func, arg, &threadQueue);
}

void threadContext::handler(thread_startfunc_t func, uintptr_t arg, std::queue<std::shared_ptr<threadContext>>& tQueue){
  // Deleting the previous finished thread - to free up memory
    if (cpu::self()->needDelete != nullptr){
        delete[] cpu::self()->needDelete->charPtr;
        cpu::self()->needDelete->charPtr = nullptr;
        delete cpu::self()->needDelete->ucontext_ptr;
        cpu::self()->needDelete->ucontext_ptr = nullptr;
        cpu::self()->needDelete = nullptr;
    }

    //Starting the user code, enable interupts during this time.
    cpu::self()->interrupt_enable();
   
    func(arg);

    //Ending the user code, disable interupts during this time.
    cpu::self()->interrupt_disable();

    //empty the thread's own wait list into the ready queue
    while(!tQueue.empty()){
        cpu::self()->ready_queue.emplace(tQueue.front());
        tQueue.pop();
    }


    //To distinguish that we have finished running the user code,
    //We will add a nullptr into the thread's queue which is now empty,
    //which will be checked later in join.
    std::shared_ptr<threadContext> tempstate = nullptr;
    tQueue.emplace(tempstate);

    //Adding ourselves to Delete, so in the next thread call of handler, I can be deleted
    cpu::self()->needDelete = cpu::self()->runningThread;


    //if the cpu is the running thread, nothing else to run in the ready queue
    // we can end with the cpu right away
    if(cpu::self()->ready_queue.empty()){
        cpu::self()->finished = true;
        cpu::self()->interrupt_enable_suspend();
    }


    //SETTING NEXT AVAILABLE THREAD
    cpu::self()->runningThread = cpu::self()->ready_queue.front();
    cpu::self()->ready_queue.pop();
    setcontext(cpu::self()->runningThread->ucontext_ptr);

}

threadContext::~threadContext() {
    //deleting the current instance of the threadContext class
   // std:: cout << "deconstructing" <<std::endl;
    //cpu::self()->interrupt_disable();
    //assert_interrupts_disabled();
    if (cpu::self()->finished){
        delete[] charPtr;
        charPtr = nullptr;
        delete ucontext_ptr;
        ucontext_ptr = nullptr;
    }

//    cpu::self()->interrupt_enable();
}