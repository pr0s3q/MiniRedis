#pragma once

#include <thread>
#include <vector>

class TaskQueue;

class ThreadPool
{
public:
    explicit ThreadPool(TaskQueue& taskQueue);

    ~ThreadPool();

    // ThreadPool cannot exist without TaskQueue
    ThreadPool() = delete;
    ThreadPool(ThreadPool const&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool const&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

private:
    std::vector<std::thread> m_threads;
    TaskQueue& m_taskQueue;
};
