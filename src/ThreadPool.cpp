#include <ThreadPool.hpp>

#include <TaskQueue.hpp>
#include <TcpConnection.hpp>

#include <thread>

ThreadPool::ThreadPool(TaskQueue& taskQueue)
    : m_taskQueue(taskQueue)
{
    //
    // Use all threads minus 2 for the rest of the OS
    // Unless, the machine have 2 or 1 thread
    //
    // hardware_concurrency can return 0, if the value is not well defined or not computable
    // Use one thread in that case
    //
    auto noOfThreads = std::thread::hardware_concurrency();
    if (noOfThreads == 0)
        noOfThreads = 1;
    else if (noOfThreads > 2)
        noOfThreads -=2;

    m_threads.reserve(noOfThreads);
    for (unsigned int i{0}; i < noOfThreads; ++i)
    {
        m_threads.emplace_back([this]
        {
            while (true)
            {
                auto clientDescriptor = m_taskQueue.Pop();
                if (!clientDescriptor.has_value())
                    return;

                TcpConnection::HandleClient(clientDescriptor.value());
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_taskQueue.Stop();

    for (auto& thread : m_threads)
        thread.join();
}
