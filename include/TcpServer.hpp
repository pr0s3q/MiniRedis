#pragma once

#include <netinet/in.h>

namespace TcpServer
{
    int Create(sockaddr_in& address);
}