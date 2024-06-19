// scheduler.cc
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would
//	end up calling FindNextToRun(), and that would put us in an
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

//<TODO>
// Declare sorting rule of SortedList for L1 & L2 ReadyQueue
// Hint: Funtion Type should be "static int"
static int CompareRemainingTime(Thread *a, Thread *b)
{
    return a->getRemainingBurstTime() - b->getRemainingBurstTime();
}

static int CompareThreadID(Thread *a, Thread *b)
{
    return a->getID() - b->getID();
}
//<TODO>

Scheduler::Scheduler()
{
    // schedulerType = type;
    // readyList = new List<Thread *>;
    //<TODO>
    L1ReadyQueue = new SortedList<Thread *>(CompareRemainingTime);
    L2ReadyQueue = new SortedList<Thread *>(CompareThreadID);
    L3ReadyQueue = new List<Thread *>;
    // Initialize L1, L2, L3 ReadyQueue
    //<TODO>
    toBeDestroyed = NULL;
}

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{
    //<TODO>
    // Remove L1, L2, L3 ReadyQueue
    delete L1ReadyQueue;
    delete L2ReadyQueue;
    delete L3ReadyQueue;
    //<TODO>
    // delete readyList;
}

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void Scheduler::ReadyToRun(Thread *thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    //DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    Statistics *stats = kernel->stats;
    //<TODO>
    // According to priority of Thread, put them into corresponding ReadyQueue.
    // After inserting Thread into ReadyQueue, don't forget to reset some values.
    // Hint: L1 ReadyQueue is preemptive SRTN(Shortest Remaining Time Next).
    // When putting a new thread into L1 ReadyQueue, you need to check whether preemption or not.
	//Thread *currentThread =  kernel->currentThread;
    if (thread->getPriority() >= 100)
    {
        L1ReadyQueue->Insert(thread);
		DEBUG('z', "[InsertToQueue] Tick [" << stats -> totalTicks << "] Thread [" << thread->getID() << "] is inserted into queue L1");
        /*if (currentThread->getRemainingBurstTime() > thread->getRemainingBurstTime())
        {
			
		DEBUG('z', "[RemoveFromQueue] Tick [" << stats -> totalTicks << "] Thread [" << kernel->currentThread->getID() << "] is removed from queue L1");	
            kernel->interrupt->YieldOnReturn();
        }*/
    }
    else if (thread->getPriority() >= 50)
    {
        L2ReadyQueue->Insert(thread);
		DEBUG('z', "[InsertToQueue] Tick [" << stats -> totalTicks << "] Thread [" << thread->getID() << "] is inserted into queue L2");
    }
    else
    {
        L3ReadyQueue->Append(thread);
		DEBUG('z', "[InsertToQueue] Tick [" << stats -> totalTicks << "] Thread [" << thread->getID() << "] is inserted into queue L3");
    }
    thread->setWaitTime(0);
    thread->setRRTime(0);
    //<TODO>
    // readyList->Append(thread);
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    /*if (readyList->IsEmpty()) {
    return NULL;
    } else {
        return readyList->RemoveFront();
    }*/

    if (!L1ReadyQueue->IsEmpty())
    {
	Thread* nextThread = L1ReadyQueue->RemoveFront();
	DEBUG('z', "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is removed from queue L1");
        return nextThread;
    }
    else if (!L2ReadyQueue->IsEmpty())
    {
	Thread* nextThread = L2ReadyQueue->RemoveFront();
	DEBUG('z',  "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is removed from queue L2");
        return nextThread;
    }
    else if (!L3ReadyQueue->IsEmpty())
    {
		Thread* nextThread = L3ReadyQueue->RemoveFront();
        DEBUG('z',"[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is removed from queue L3");
        return nextThread;
    }
    return NULL;

    //<TODO>
    // a.k.a. Find Next (Thread in ReadyQueue) to Run
    //<TODO>
}



//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void Scheduler::Run(Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentThread;

    //cout << "Current Thread" << oldThread->getName() << "    Next Thread" << nextThread->getName() << endl;

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing)
    { // mark that we need to delete current thread
        ASSERT(toBeDestroyed == NULL);
        toBeDestroyed = oldThread;
    }

#ifdef USER_PROGRAM // ignore until running user programs
    if (oldThread->space != NULL)
    { // if this thread is a user program,

        oldThread->SaveUserState(); // save the user's CPU registers
        oldThread->space->SaveState();
    }
#endif

    oldThread->CheckOverflow(); // check if the old thread
                                // had an undetected stack overflow

    kernel->currentThread = nextThread; // switch to the next thread
    nextThread->setStatus(RUNNING);     // nextThread is now running

    // int executedTicks = kernel->stats->totalTicks - oldThread->getStartTick();
    // int oldBurstTime = oldThread->getRemainingBurstTime();
    // oldThread->setRemainingBurstTime(oldBurstTime - executedTicks);

    // DEBUG('z', std::cout << "[UpdateRemainingBurstTime] Tick [" << kernel->stats->totalTicks << "]: Thread [" << oldThread->getID() << "] update remaining burst time, from: [" << oldBurstTime << "] - [" << executedTicks << "], to [" << oldThread->getRemainingBurstTime() << "]\n");

    // nextThread->setStartTick(kernel->stats->totalTicks); // Set start tick for the new thread

    // DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".
	//DEBUG('z', cout << "[ContextSwitch] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is now selected for execution, thread [" << oldThread->getID() << "] is replaced, and it has executed [" << kernel->stats->totalTicks - oldThread->getStartTick() << "] ticks\n");

    DEBUG('z',"[ContextSwitch] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is now selected for execution, thread [" << oldThread->getID() << "] is replaced, and it has executed [" << kernel->stats->totalTicks - oldThread->getStartTick() << "] ticks");
    DEBUG('z',  "Switching from: " << oldThread->getID() << " to: " << nextThread->getID());
    SWITCH(oldThread, nextThread);

    // we're back, running oldThread

    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    /*DEBUG(dbgThread, "Now in thread: " << kernel->currentThread->getID());*/

    CheckToBeDestroyed(); // check if thread we were running
                          // before this one has finished
                          // and needs to be cleaned up

#ifdef USER_PROGRAM
    if (oldThread->space != NULL)
    {                                  // if there is an address space
        oldThread->RestoreUserState(); // to restore, do it.
        oldThread->space->RestoreState();
    }
#endif
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL)
    {
        DEBUG(dbgThread, "toBeDestroyed->getID(): " << toBeDestroyed->getID());
        delete toBeDestroyed;
        toBeDestroyed = NULL;
    }
}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void Scheduler::Print()
{
    cout << "Ready list contents:\n";
    // readyList->Apply(ThreadPrint);
    cout << "L1 Queue contents:\n";
    L1ReadyQueue->Apply(ThreadPrint);
    cout << "L2 Queue contents:\n";
    L2ReadyQueue->Apply(ThreadPrint);
    cout << "L3 Queue contents:\n";
    L3ReadyQueue->Apply(ThreadPrint);
}

// <TODO>

// Function 1. Function definition of sorting rule of L1 ReadyQueue

// Function 2. Function definition of sorting rule of L2 ReadyQueue

// Function 3. Scheduler::UpdatePriority()

// Hint:
// 1. ListIterator can help.
// 2. Update WaitTime and priority in Aging situations
// 3. After aging, Thread may insert to different ReadyQueue

void Scheduler::UpdatePriority()
{
	// Update WaitTime
	ListIterator<Thread *> *iter;
	iter = new ListIterator<Thread *>(L3ReadyQueue);
	for (;!iter->IsDone(); iter->Next()) {
		Thread *thread = iter->Item();
		thread->setWaitTime(thread->getWaitTime() + 100);
	}
		
	delete iter;

	iter = new ListIterator<Thread *>(L2ReadyQueue);
	for (;!iter->IsDone(); iter->Next()) {
		Thread *thread = iter->Item();
		thread->setWaitTime(thread->getWaitTime() + 100);
	}

	delete iter;

	iter = new ListIterator<Thread *>(L1ReadyQueue);
	for (;!iter->IsDone(); iter->Next()) {
		Thread *thread = iter->Item();
		thread->setWaitTime(thread->getWaitTime() + 100);
	}
	delete iter;
		
	Thread* currentThread = kernel->currentThread;

	
	
    //  Aging : increase 10 after more than 400 ticks

    iter = new ListIterator<Thread *>(L3ReadyQueue);
    for (; !iter->IsDone(); iter->Next())
    {
        Thread *thread = iter->Item();
        if (thread->getWaitTime() >= 400) {
			int oldPriority = thread->getPriority();
			thread->setPriority(oldPriority + 10);
	    	DEBUG('z',  "[UpdatePriority] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] changes its priority from [" << oldPriority << "] to [" << thread->getPriority() << "]");
			thread->setWaitTime(0);
	   		if (thread->getPriority() >= 50) {
            	L3ReadyQueue->Remove(thread);
            	ReadyToRun(thread);
			}
        }
    }
    delete iter;

    iter = new ListIterator<Thread *>(L2ReadyQueue);
    for (; !iter->IsDone(); iter->Next())
    {
        Thread *thread = iter->Item();
        if (thread->getWaitTime() >= 400) {
			int oldPriority = thread->getPriority();
			thread->setPriority(oldPriority+10);	
			DEBUG('z', "[UpdatePriority] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] changes its priority from [" << oldPriority << "] to [" << thread->getPriority() << "]");
			thread->setWaitTime(0);
			if (thread->getPriority() >= 100) {
				L2ReadyQueue->Remove(thread);
				ReadyToRun(thread);
			}
		}
    }
    delete iter;

    iter = new ListIterator<Thread *>(L1ReadyQueue);
    for (; !iter->IsDone(); iter->Next())
    {
        Thread *thread = iter->Item();
        if (thread->getWaitTime() >= 400 && (thread->getPriority() < 149))
        {
			int oldPriority = thread->getPriority();
			if (oldPriority + 10 > 149) { // the highest priority is 149
				thread->setPriority(149);	
			}
			else {
				thread->setPriority(oldPriority+10);
			}
			thread->setWaitTime(0);
	   		DEBUG('z',"[UpdatePriority] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] changes its priority from [" << oldPriority << "] to [" << thread->getPriority() << "]");
            //L1ReadyQueue->Remove(thread);
			//ReadyToRun(thread);
        } 
    }
    delete iter;
	
	if (currentThread->getPriority() >= 100) {
		if (L1ReadyQueue->Front()->getRemainingBurstTime() < currentThread->getRemainingBurstTime()){
			//kernel->interrupt->YieldOnReturn();	
		}
	}
	else if (currentThread->getPriority() >= 50) {
		if (!L1ReadyQueue->IsEmpty()) {
			//kernel->interrupt->YieldOnReturn();
			//cout << L1ReadyQueue->Front()->getID() << "\n";
		}
	}
	else {
		// Update RRTime and Check RR
		currentThread->setRRTime(currentThread->getRRTime() + 100);
		if (!L1ReadyQueue->IsEmpty() || !L2ReadyQueue->IsEmpty()) {
			//kernel->interrupt->YieldOnReturn();
		}
		else if (currentThread->getRRTime() >= 200 && !L3ReadyQueue->IsEmpty()) {
			currentThread->setRRTime(0);
			//kernel->interrupt->YieldOnReturn();
		}
	}
}

// <TODO>
