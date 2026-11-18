#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

class TaskQueue
{
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    void Push(int clientDescriptor);
    std::optional<int> Pop();
    void Stop();

    TaskQueue(TaskQueue const& other) = delete;
    TaskQueue(TaskQueue&& other) = delete;
    TaskQueue& operator=(TaskQueue const& other) = delete;
    TaskQueue& operator=(TaskQueue&& other) = delete;

private:
    std::queue<int> m_clientDescriptorQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    bool m_stop{};
};
