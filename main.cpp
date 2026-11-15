#include <cstdio>
#include <iostream>
#include <thread>

#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>

//
// Docs from
// https://pubs.opengroup.org/onlinepubs/9799919799/
//

namespace
{
    void Close(int);

    int HandleError(const int fileDescriptor, const char* str)
    {
        perror(str);
        Close(fileDescriptor);
        return -1;
    }

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
            perror("close");
    }

    void HandleClient(const int clientFileDescriptor)
    {
        //
        // recv - receive a message from a connected socket
        //
        // why "sizeof(buffer) - 1"
        // The - 1 leaves the last byte as the 0 from {},
        // because recv() does not add a terminator and "std::cout << buffer" needs one.
        //
        // Return - Upon successful completion, recv() shall return the length of the message in bytes.
        // If no messages are available to be received and the peer has performed an orderly shutdown, recv() shall return 0.
        // Otherwise, -1 shall be returned and errno set to indicate the error.
        //
        char buffer[256]{};
        const ssize_t receiveResult = recv(clientFileDescriptor, buffer, sizeof(buffer) - 1, 0);
        if (receiveResult < 0)
        {
            HandleError(clientFileDescriptor, "recv");
            return;
        }

        if (receiveResult == 0)
        {
            Close(clientFileDescriptor);
            return;
        }

        std::cout << buffer << '\n';

        //
        // send - send a message on a socket
        //
        // MSG_NOSIGNAL - Requests not to send the SIGPIPE signal if an attempt to send is made on a
        // stream-oriented socket that is no longer connected. The [EPIPE] error shall still be returned.
        //
        // Return - Upon successful completion, send() shall return the number of bytes sent.
        // Otherwise, -1 shall be returned and errno set to indicate the error.
        //
        constexpr char successfulResponse[] = "OK\n";
        constexpr std::size_t successfulResLen = sizeof(successfulResponse) - 1;
        const ssize_t sendResult = send(clientFileDescriptor, successfulResponse, successfulResLen, MSG_NOSIGNAL);
        if (sendResult < 0)
        {
            HandleError(clientFileDescriptor, "send");
            return;
        }

        if (sendResult < successfulResLen)
        {
            std::cerr << "send: partial write, sent " << sendResult
                      << " of " << successfulResLen << " bytes\n";
            Close(clientFileDescriptor);
            return;
        }

        Close(clientFileDescriptor);
    }
}

int main()
{
    //
    // socket - create an endpoint for communication
    //
    // AF_INET - Internet domain sockets for use with IPv4 addresses.
    // SOCK_STREAM - Byte-stream socket
    // Protocol 0 - causes socket() to use an unspecified default protocol appropriate for the requested socket type.
    //
    // Return - Upon successful completion, socket() shall return a non-negative integer, the socket file descriptor.
    // Otherwise, a value of -1 shall be returned and errno set to indicate the error.
    //
    const int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0)
    {
        perror("socket");
        return fileDescriptor;
    }

    //
    // IPv4 address
    // Port 60000
    // INADDR_LOOPBACK - 127.0.0.1
    //
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(60000);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    //
    // bind - bind a name to a socket
    //
    // Return - Upon successful completion, bind() shall return 0;
    // otherwise, -1 shall be returned and errno set to indicate the error.
    //
    const int bindRes = bind(fileDescriptor, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (bindRes < 0)
        return HandleError(fileDescriptor, "bind");

    //
    // listen - listen for socket connections and limit the queue of incoming connections
    //
    // Return - Upon successful completions, listen() shall return 0;
    // otherwise, -1 shall be returned and errno set to indicate the error.
    //
    const int listenResult = listen(fileDescriptor, 2);
    if (listenResult < 0)
        return HandleError(fileDescriptor, "listen");

    //
    // accept - accept a new connection on a socket
    //
    // Return - Upon successful completion, accept() and accept4() shall return the non-negative file descriptor of the accepted socket.
    // Otherwise, -1 shall be returned, errno shall be set to indicate the error, and any object pointed to by address_len shall remain unchanged.
    //
    while (true)
    {
        socklen_t addressLength = sizeof(address);
        const int socketFileDescriptor = accept(fileDescriptor, reinterpret_cast<sockaddr*>(&address), &addressLength);
        if (socketFileDescriptor < 0)
            return HandleError(fileDescriptor, "accept");

        std::thread thread(HandleClient, socketFileDescriptor);
        thread.detach();
    }
}
