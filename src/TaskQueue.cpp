#include <TaskQueue.hpp>

void TaskQueue::Push(const int clientDescriptor)
{
    std::unique_lock lock{ m_queueMutex };
    m_clientDescriptorQueue.push(clientDescriptor);

    lock.unlock();
    m_cv.notify_one();
}

std::optional<int> TaskQueue::Pop()
{
    // Wait until notified, and until queue have data
    std::unique_lock lock{ m_queueMutex };
    m_cv.wait(lock, [this]{ return m_stop || !m_clientDescriptorQueue.empty(); });

    if (m_clientDescriptorQueue.empty())
        return std::optional<int>{ std::nullopt };

    const int clientDescriptor = m_clientDescriptorQueue.front();
    m_clientDescriptorQueue.pop();
    lock.unlock();

    return std::optional{ clientDescriptor };
}

void TaskQueue::Stop()
{
    std::unique_lock lock{m_queueMutex};
    m_stop = true;
    lock.unlock();
    m_cv.notify_all();
}
