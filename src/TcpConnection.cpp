#include <TcpConnection.hpp>

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <Logger.hpp>

namespace
{
    void Close(const int fileDescriptor)
    {
        //
        // close - close a file descriptor
        //
        // Return - Upon successful completion, 0 shall be returned;
        // otherwise, -1 shall be returned and errno set to indicate the error.
        //
        const int closeResult = close(fileDescriptor);
        if (closeResult < 0)
            Logger::LogError("close");
    }

    int SendResponse(const int fileDescriptor)
    {
        //
        // send - send a message on a socket
        //
        // MSG_NOSIGNAL - Requests not to send the SIGPIPE signal if an attempt to send is made on a
        // stream-oriented socket that is no longer connected. The [EPIPE] error shall still be returned.
        //
        // Return - Upon successful completion, send() shall return the number of bytes sent.
        // Otherwise, -1 shall be returned and errno set to indicate the error.
        //
        constexpr std::string_view successfulResponse = "OK\n";
        constexpr std::size_t successfulResLen = successfulResponse.size();
        const ssize_t sendResult = send(fileDescriptor, successfulResponse.data(), successfulResLen, MSG_NOSIGNAL);
        if (sendResult < 0)
        {
            TcpConnection::HandleError(fileDescriptor, "send");
            return -1;
        }

        if (std::cmp_less(sendResult, successfulResLen))
        {
            Logger::LogError("send: partial write, sent {} of {} bytes", sendResult, successfulResLen);
            Close(fileDescriptor);
            return -1;
        }

        return 0;
    }
}

namespace TcpConnection
{
    void HandleClient(const int clientFileDescriptor)
    {
        while (true)
        {
            //
            // recv - receive a message from a connected socket
            //
            // Return - Upon successful completion, recv() shall return the length of the message in bytes.
            // If no messages are available to be received and the peer has performed an orderly shutdown, recv() shall return 0.
            // Otherwise, -1 shall be returned and errno set to indicate the error.
            //
            constexpr std::size_t bufferSize {256};
            std::array<char, bufferSize> buffer{};
            std::string message{};
            std::string command{};
            message.reserve(bufferSize);
            while (true)
            {
                const ssize_t receiveResult = recv(clientFileDescriptor, buffer.data(), sizeof(buffer), 0);
                if (receiveResult < 0)
                {
                    HandleError(clientFileDescriptor, "recv");
                    return;
                }

                if (receiveResult == 0)
                {
#ifdef LOGS
                    Logger::LogSuccess("Client disconnected");
#endif
                    Close(clientFileDescriptor);
                    return;
                }

                message.append(buffer.data(), static_cast<std::size_t>(receiveResult));

                std::size_t pos = message.find('\n');
                if (pos == std::string::npos)
                {
                    // No new line character - call recv again
#ifdef LOGS
                    Logger::LogInformation("Partial message: {}", message);
#endif
                    command.append(message);
                    message.clear();
                    continue;
                }

                // New line character
                while (pos != std::string::npos)
                {
                    command.append(message.substr(0, pos));
                    message.erase(0, pos + 1);
                    Logger::LogMessage("{}", command);

                    // Don't close the file descriptor if SendResponse returns -1 (failure)
                    // SendResponse have implemented error handling and connection closing
                    if (SendResponse(clientFileDescriptor) < 0)
                        return;

                    command.clear();

                    if (message.empty())
                        break;

                    pos = message.find('\n');
                }

                if (!message.empty())
                    continue;

                break;
            }
        }
    }

    int HandleError(const int fileDescriptor, const char* str)
    {
        Logger::LogError(str);
        Close(fileDescriptor);
        return -1;
    }
}
