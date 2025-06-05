#pragma once

#if AXE_OS_UNIX

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> // struct sockaddr_in

#endif // AXE_OS_UNIX