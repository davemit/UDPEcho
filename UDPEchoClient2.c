/*********************************************************
* Module Name: UDP Echo client source 
*
* File Name:    UDPEchoClient2.c
*
* Summary:
*  This file contains the echo Client code.
*
* 
*
*********************************************************/
#include "UDPEcho.h"
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>

void clientCNTCCode();
int numberOfTimeOuts=0;
unsigned int numberOfTrials;
unsigned long bytesSent;
unsigned long avgSendingRate;
long totalPing;
int bStop;

char Version[] = "1.1";   

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* IP address of server */
    char *packetData;                /* String to send to echo server */
    struct hostent *thehost;	     /* Hostent from gethostbyname() */
    struct timeval *theTime1;
    struct timeval *theTime2;
    struct timeval *theTime3;
    struct timeval TV1, TV2, TV3;
    struct sigaction myaction;
    long usec1 = 0, usec2, usec3 = 0;
    int *seqNumberPtr;
    unsigned int seqNumber = 1;
    theTime1 = &TV1;
    theTime2 = &TV2;
    theTime3 = &TV3;

    //Initialize values
    numberOfTimeOuts = 0;
    bytesSent = 0;
    avgSendingRate = 0;
    totalPing =0;
    unsigned long averageRate = 0;
    unsigned int bucketSize = 0;
    unsigned int messageSize = 0;
    numberOfTrials = 0;
    bStop = 0;

    if (argc != 6)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Server Port> <Average Rate> <Bucket Size> <Message Size\n", argv[0]);
        exit(1);
    }

  signal (SIGINT, clientCNTCCode);

  servIP = argv[1];           /* First arg: server IP address (dotted quad) */

  /* get info from parameters , or default to defaults if they're not specified */
  echoServPort = atoi(argv[2]);
  averageRate = atoi(argv[3]);
  bucketSize = atoi(argv[4]);
  messageSize = atoi(argv[5]);

  if (sigaction(SIGALRM, &myaction, 0) < 0)
    DieWithError("sigaction failed for sigalarm");

  /* Set up the echo string */
  packetData = malloc(messageSize);
  memset(packetData, '1', messageSize);

  seqNumberPtr = (int *)packetData;

  /* Construct the server address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
  echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    
    /* If user gave a dotted decimal address, we need to resolve it  */
  if (echoServAddr.sin_addr.s_addr == -1) {
    thehost = gethostbyname(servIP);
	  echoServAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
  }
    
  echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
  int *bucket = malloc(sizeof(int));
  *bucket = 0;
    /* Create a datagram/UDP socket */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");
  gettimeofday(theTime1, NULL);
  while (1) {

    if (bStop == 1){
      //CTRL-C received, prepare output message
      gettimeofday(theTime2, NULL); //Check timer for deciding whether to fill bucket
      usec2 = (theTime2->tv_sec) * 1000000 + (theTime2->tv_usec);
      usec2 -= usec3;
      
      usec2 /= 1000000; 
      if (numberOfTrials != 0) 
        avgSendingRate = ((bytesSent) / (usec2));
      else 
        avgSendingRate = 0;
      printf("\n%lu %lu\n", bytesSent, avgSendingRate);
      exit(0);
    }

    *seqNumberPtr = htonl(seqNumber++); 

    if(*bucket >= messageSize){
      if(usec3 == 0){
//        printf("set start time\n");
        gettimeofday(theTime3, NULL); //Check timer for deciding whether to fill bucket
        usec3 = (theTime3->tv_sec) * 1000000 + (theTime3->tv_usec);
      }
      if (sendto(sock, packetData, messageSize, 0, (struct sockaddr *)
                &echoServAddr, sizeof(echoServAddr)) != messageSize)
        DieWithError("sendto() sent a different number of bytes than expected");
//        printf("sent packet\n");
//	fflush(stdout);
        bytesSent += messageSize; //Increment counter of bytes transmitted
        *bucket -= messageSize;   //Decrement bytes available in bucket
        numberOfTrials++;         //Increment number of packets sent
    }

    gettimeofday(theTime2, NULL); //Check timer for deciding whether to fill bucket

    usec2 = (theTime2->tv_sec) * 1000000 + (theTime2->tv_usec);
    usec1 = (theTime1->tv_sec) * 1000000 + (theTime1->tv_usec);

    usec2 = (usec2 - usec1);
    //Check if 100 microseconds have passed, use this to fill bucket
    if(usec2 >= 1000){
      //increment bucket by 1/1000 every millisecond
      //add 999 to make sure to round up so we don't lose bits
      *bucket += (averageRate + 999) / 1000;
      //cap bucket level at max bucket size
      if(*bucket > bucketSize){
        *bucket = bucketSize;
      }
      //reset timer
      usec2 = 0;
      //start next timer
      gettimeofday(theTime1, NULL);
    }

  }
  close(sock);
  exit(0);
}


void clientCNTCCode() {
  bStop = 1;
}


