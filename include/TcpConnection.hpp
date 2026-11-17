#pragma once

namespace TcpConnection
{
    void HandleClient(int clientFileDescriptor);

    int HandleError(int fileDescriptor, const char* str);
}