/*********************************************************
*
* Module Name: UDP Echo server 
*
* File Name:    UDPEchoServer.c	
*
* Summary:
*  This file contains the echo server code
*
*
*********************************************************/
#include "UDPEcho.h"
#include <signal.h>
#include <sys/time.h>
void DieWithError(char *errorMessage);  /* External error handling function */

typedef struct connectionDetails{
  struct in_addr ip;
  unsigned int port;
  unsigned long dataRx;
  unsigned int totalSessions;
  unsigned long start;
  unsigned long last;
} connDetails;

void clientCNTCCode();

char Version[] = "1.1";   
connDetails *conns;
char bStop = 0;
int numberOfConnections = 0;

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    struct timeval *theTime;
    struct timeval tv;
    theTime = &tv;
    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    //Set signal handler so we can exit
    signal(SIGINT, clientCNTCCode);

    //Allocate space for storing our connections
    conns = malloc(sizeof(connDetails) * 100);
    memset(conns, '\0', sizeof(connDetails) * 100);

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
      printf("Failure on socket call , errno:%d\n",errno);
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
      printf("Failure on bind, errno:%d\n",errno);
    }
    
    int i = 0;
    char bNewConnection = 1;

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        {
          printf("Failure on recvfrom, client: %s, errno:%d\n", 
              inet_ntoa(echoClntAddr.sin_addr),errno);
        } else {
            //Record time for calculating latest received time
            gettimeofday(theTime, NULL);
            //Check for match with current connections
            for(i = 0; i < numberOfConnections; i++){
              //If we find a match, go ahead and add in the data
              if(conns[i].ip.s_addr == echoClntAddr.sin_addr.s_addr &&
                 conns[i].port == echoClntAddr.sin_port){
                conns[i].dataRx += recvMsgSize;
                conns[i].totalSessions++;
                conns[i].last = (theTime->tv_sec) * 1000000 
                                 + (theTime->tv_usec); 
                bNewConnection = 0;
              }
            }
            //If we don't find a match, add it to our connections
            if(bNewConnection){
              conns[numberOfConnections].ip = echoClntAddr.sin_addr;
              conns[numberOfConnections].port = echoClntAddr.sin_port;
              conns[numberOfConnections].dataRx = recvMsgSize;
              conns[numberOfConnections].totalSessions = 1;
              conns[numberOfConnections].start = (theTime->tv_sec) * 1000000 
                              + (theTime->tv_usec); 
              conns[numberOfConnections].last = (theTime->tv_sec) * 1000000 
                              + (theTime->tv_usec);
              numberOfConnections++;
              bNewConnection = 0;
 
            }
            //Reset so we can handle new connection next time
            bNewConnection = 1;
        }

        

    }
    /* NOT REACHED */
}

void clientCNTCCode(){
  int i = 0;
  float elapsedTime = 0.;
  char ipaddr[INET_ADDRSTRLEN + 1];
  unsigned long throughput = 0;
  for(i = 0; i < numberOfConnections; i++){
    throughput += conns[i].dataRx;
  }

  printf("%d %lu\n", numberOfConnections, throughput);
  for(i = 0; i < numberOfConnections; i++){
    inet_ntop(AF_INET, &(conns[i].ip), ipaddr, INET_ADDRSTRLEN + 1);
    elapsedTime = (float)(conns[i].last - conns[i].start) / 1000000.;
    if(elapsedTime <= 0){
      printf("too little time recorded\n");
    }
    printf("%s %u %.3lf %lu %lu\n",
    ipaddr,
    conns[i].port,
    elapsedTime,
    conns[i].dataRx,
    (unsigned long) conns[i].dataRx / (unsigned long)(elapsedTime));
  }
  exit(0);
}
