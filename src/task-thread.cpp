#include "task-thread.h"

#include <pthread.h>
#include <signal.h>

#include <iostream>

// xxx is instantiating thread in constructor going to have issues? LOOK INTO!
TaskThread::TaskThread() :
    _wakeup(false),
    _stop(false),
    _th()
    // xxx _th should already be instantiated upon object creation (?) others?
{}

// xxx review sfml game-dev book for implentation of locking
// xxx template<typename ReturnType>
/**
 * Private method that accepts a std::packaged_task function and invokes it.
 */
template <typename ReturnType>
void TaskThread::run_task(std::packaged_task<ReturnType ()>&& task)
{
    task();
}

/**
 * Run bool async Task (return FALSE when finished).
 * @note Pass void anonymous function (as lambda), but std::packaged_task 
 * expects bool. Underlying expected function is bool.
 */
void TaskThread::async(std::function<void ()> task)
{
    // assume task completion means TRUE
    std::packaged_task<bool ()> ptask([task]() -> bool {
        // xxx maybe want this?
        //if (task_) {
        //    task_(); // Execute the task if it's not empty
        //    return true;
        //} else {
        //    return false; // Return false if the task is empty
        //}

        task();
        return true;
    });

    _fut = ptask.get_future();
    /// xxx 2 threads of no? look into (!)
    _th = std::thread(&TaskThread::run_task<bool>, this, std::move(ptask));
    _th.detach();

    /// xxx look into this (?)
    //{
    //    auto lock = std::unique_lock<std::mutex>(_mtx);
    //    _finished = false;
    //}
}

/**
 * @return TRUE if async Task is finished.
 */
bool TaskThread::async_is_finished()
{
    auto lock = std::unique_lock<std::mutex>(_mtx);
    if (_fut.get() != false)
        return false;

    return true;
}

/**
 * Wakeup TaskThread.
 */
void TaskThread::wakeup() 
{
    auto lock = std::unique_lock<std::mutex>(_wakemtx);
    _wakeup = true;
    _wakecond.notify_one();
}

/**
* Wait for TaskThread to complete.
*/
void TaskThread::wait() 
{
    auto lock = std::unique_lock<std::mutex>(_waitmtx);
    while (_wakeup) {
        _wakecond.wait(lock);
    }
}

/**
* Ask the TaskThread to stop.
*/
void TaskThread::stop() 
{
    auto lock = std::unique_lock<std::mutex>(_wakemtx);
    _stop = true;
    _wakecond.notify_one();
}

/**
* Join the TaskThread (wait for it to stop and join).
*/
void TaskThread::join()
{
    _th.join();
}

/** 
 * @warning Native kill procedure only! (Linux)
 * Call native kill procedure (pthread_kill()).
 */
void TaskThread::native_cancel()
{
    pthread_t pth = _th.native_handle();
    std::cout << "Native handle of thread recieved!\n"
        << "Sending SIGKILL to thread...\n";
    //pthread_kill(pth, SIGKILL);
    pthread_cancel(NULL);
}

/**
* Upon object destruction, cleanup and join thread back to main.
*/
TaskThread::~TaskThread()
{
    if (_th.joinable())
        _th.join();
}
