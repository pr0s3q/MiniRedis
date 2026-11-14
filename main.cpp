#include <cstdio>

#include <sys/socket.h>

#include <netinet/in.h>

int main()
{
    //
    // Docs from
    // https://pubs.opengroup.org/onlinepubs/7908799/index.html
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
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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
    int bindRes = bind(fileDescriptor, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (bindRes < 0)
    {
        perror("bind");
        return bindRes;
    }

    return 0;
}
