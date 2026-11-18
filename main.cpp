#include <netinet/in.h>
#include <sys/socket.h>
// NOLINTNEXTLINE(modernize-deprecated-headers)
#include <signal.h>

#include <cerrno>
#include <csignal>

#include <TcpConnection.hpp>
#include <TcpServer.hpp>
#include <TaskQueue.hpp>
#include <ThreadPool.hpp>
#include <Logger.hpp>

//
// Docs from
// https://pubs.opengroup.org/onlinepubs/9799919799/
//

namespace
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    std::sig_atomic_t stopRequested = 0;

    void OnSignal(int /*unused*/)
    {
        stopRequested = 1;
    }
}

int main()
{
    sockaddr_in address{};
    const int fileDescriptor = TcpServer::Create(address);
    if (fileDescriptor < 0)
        return fileDescriptor;

    struct sigaction action{};
    action.sa_handler = OnSignal;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);

    //
    // accept - accept a new connection on a socket
    //
    // Return - Upon successful completion, accept() and accept4() shall return the non-negative file descriptor of the accepted socket.
    // Otherwise, -1 shall be returned, errno shall be set to indicate the error, and any object pointed to by address_len shall remain unchanged.
    //

    TaskQueue taskQueue{};
    ThreadPool threadPool{ taskQueue };

    while (true)
    {
        socklen_t addressLength = sizeof(address);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const int socketFileDescriptor = accept(fileDescriptor, reinterpret_cast<sockaddr*>(&address), &addressLength);
        if (socketFileDescriptor < 0)
        {
            if (errno == EINTR && stopRequested != 0)
            {
                Logger::LogInformation("Server terminated!");
                break;
            }

            return TcpConnection::HandleError(fileDescriptor, "accept");
        }

        taskQueue.Push(socketFileDescriptor);
    }
}
