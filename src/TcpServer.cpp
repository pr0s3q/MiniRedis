#include <TcpServer.hpp>

#include <cstdio>

#include <netinet/in.h>

#include <sys/socket.h>

#include <TcpConnection.hpp>

namespace TcpServer
{
    int Create(sockaddr_in& address)
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
            return TcpConnection::HandleError(fileDescriptor, "bind");

        //
        // listen - listen for socket connections and limit the queue of incoming connections
        //
        // Return - Upon successful completions, listen() shall return 0;
        // otherwise, -1 shall be returned and errno set to indicate the error.
        //
        const int listenResult = listen(fileDescriptor, 2);
        if (listenResult < 0)
            return TcpConnection::HandleError(fileDescriptor, "listen");

        return fileDescriptor;
    }
}
