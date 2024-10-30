/**
* @see https://stackoverflow.com/a/18385302
* https://creativecommons.org/licenses/by-sa/3.0/
* Changes were made.
*/

#pragma once

#include <mutex>
#include <thread>
#include <future>
#include <condition_variable>
#include <atomic>
#include <functional>

class TaskThread {
public:
    TaskThread();
    ~TaskThread();

    void async(std::function<void ()> task);
    bool async_is_finished();
    void join();
    void stop();
    void wait();
    void wakeup();
    void native_cancel();
private:
    template <typename ReturnType>
    void run_task(std::packaged_task<ReturnType ()>&& task);

    std::thread _th;
    std::shared_future<bool> _fut;
    std::mutex _mtx;
    std::condition_variable _cv;

    // wake up the thread
    std::atomic<bool> _wakeup;
    bool _stop;
    std::mutex _wakemtx;
    std::condition_variable _wakecond;

    // wait for the thread to finish its task
    std::mutex _waitmtx;
    std::condition_variable _waitcond;
};
