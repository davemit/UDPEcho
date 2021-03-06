/*********************************************************
*
* Module Name: UDP Echo client/server header file
*
* File Name:    UDPEcho.h	
*
* Summary:
*  This file contains common stuff for the client and server
*
*
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>     /* for memset() */
#include <netinet/in.h> /* for in_addr */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for close() */


#define ECHOMAX 10000     /* Longest string to echo */

#ifndef LINUX
#define INADDR_NONE  0xffffffff
#endif

void DieWithError(char *errorMessage);  /* External error handling function */

