#include <cstdio>
#include <iostream>

#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>

namespace
{
    int Close(const int fileDescriptor)
    {
        const int closeResult = close(fileDescriptor);
        if (closeResult < 0)
            perror("close");

        return closeResult;
    }
}

int main()
{
    //
    // Docs from
    // https://pubs.opengroup.org/onlinepubs/7908799/index.html
    //

    //
    // socket - create an endpoint for communication
    //
    // AF_INET - Internet domain sockets (IPv4)
    // SOCK_STREAM - Byte-stream socket
    // Protocol 0 - causes socket() to use an unspecified default protocol appropriate for the requested socket type.
    //
    // Return - Upon successful completion, socket() returns a nonnegative integer, the socket file descriptor.
    // Otherwise a value of -1 is returned and errno is set to indicate the error.
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
    // Return - Upon successful completion, bind() returns 0.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    const int bindRes = bind(fileDescriptor, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (bindRes < 0)
    {
        perror("bind");
        Close(fileDescriptor);
        return -1;
    }

    //
    // listen - listen for socket connections and limit the queue of incoming connections
    //
    // Return - Upon successful completions, listen() returns 0.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    const int listenResult = listen(fileDescriptor, 1);
    if (listenResult < 0)
    {
        perror("listen");
        Close(fileDescriptor);
        return -1;
    }

    //
    // accept - accept a new connection on a socket
    //
    // Return - Upon successful completion, accept() returns the nonnegative file descriptor of the accepted socket.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    socklen_t addressLength = sizeof(address);
    const int socketFileDescriptor = accept(fileDescriptor, reinterpret_cast<sockaddr*>(&address), &addressLength);
    if (socketFileDescriptor < 0)
    {
        perror("accept");
        Close(fileDescriptor);
        return -1;
    }

    //
    // recv - receive a message from a connected socket
    //
    // why "sizeof(buffer) - 1"
    // The - 1 leaves the last byte as the 0 from {},
    // because recv() does not add a terminator and "std::cout << buffer" needs one.
    //
    // Return - Upon successful completion, recv() returns the length of the message in bytes.
    // If no messages are available to be received and the peer has performed an orderly shutdown, recv() returns 0.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    char buffer[256]{};
    const ssize_t receiveResult = recv(socketFileDescriptor, buffer, sizeof(buffer) - 1, 0);
    if (receiveResult < 0)
    {
        perror("recv");
        Close(socketFileDescriptor);
        Close(fileDescriptor);
        return -1;
    }
    if (receiveResult == 0)
    {
        bool failed = false;
        int closeResult = Close(socketFileDescriptor);
        if (closeResult < 0)
            failed = true;

        closeResult = Close(fileDescriptor);
        if (closeResult < 0)
            failed = true;

        if (failed)
            return -1;

        return 0;
    }

    std::cout << buffer << '\n';

    //
    // send - send a message on a socket
    //
    // Return - Upon successful completion, send() returns the number of bytes sent.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    constexpr char successfulResponse[] = "OK\n";
    constexpr std::size_t successfulResLen = sizeof(successfulResponse) - 1;
    const ssize_t sendResult = send(socketFileDescriptor, successfulResponse, successfulResLen, 0);
    if (sendResult < 0)
    {
        perror("send");
        Close(socketFileDescriptor);
        Close(fileDescriptor);
        return -1;
    }

    if (sendResult < successfulResLen)
    {
        std::cerr << "send: partial write, sent " << sendResult
                  << " of " << successfulResLen << " bytes\n";
        Close(socketFileDescriptor);
        Close(fileDescriptor);
        return -1;
    }

    //
    // close - close a file descriptor
    //
    // Return - Upon successful completion, 0 is returned.
    // Otherwise, -1 is returned and errno is set to indicate the error.
    //
    bool failed = false;
    int closeResult = Close(socketFileDescriptor);
    if (closeResult < 0)
        failed = true;

    closeResult = Close(fileDescriptor);
    if (closeResult < 0)
        failed = true;

    if (failed)
        return -1;

    return 0;
}
