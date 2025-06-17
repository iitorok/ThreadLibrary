# ThreadLibrary
Using the Makefile, configure the target program you want to compile and compile it and the thread library files with "make". Then, simply run the target program executable. This library enables users to create
multi-threaded applications.

This repository contains the following executables that make up the thread library: 

* <h2>thread.cpp - Thread functionality</h2>
  <h3>Constructor (thread::thread(thread_startfunc_t func, uintptr_t arg))</h3>
      Uses "contextptr", a global shared_ptr of type threadContext and sets it to a newly created pointer using the func and arg parameters.
      Then, we add the newly created Thread Control Block to a global ready queue of threads.
  <h3>Yield (thread::yield)</h3>
  <h3>Join (thread::join)</h3>

* <h2>threadContext.cpp - A Thread Handler Class that saves and fetches states using Linux Ucontext calls</h2>
  <h3>Constructors (threadContext::threadContext)</h3>
  <h3>Wrapper Function (threadContext::handler)</h3>
  <h3>Destructor (threadContext::~threadContext)</h3>

* <h2>cpu.cpp - CPU functionality</h2>
  <h3>Interrupt Handler</h3>
  <h3>Constructor (cpu::cpu)</h3>
  
* <h2>mutex.cpp - Mutex functionality</h2>
  <h3>Lock()</h3>
  <h3>Unlock()</h3>

* <h2>cv.cpp - Condition Variable Functionality</h2>
  <h3>Wait(mutex& m)</h3>
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
  
  <h3>Signal()</h3>
      If the cv waiting queue is empty, this operation does nothing. 
      If not, we emplace the thread at the front of the cv's waiting queue onto the back of the ready_queue, 
      then pop the element off of the waiting queue.
  <h3>Broadcast()</h3>
      If the cv waiting queue is empty, this operation does nothing. 
      If not, we pop every item off of the cv's waiting queue and place it onto the cpu's ready queue.
