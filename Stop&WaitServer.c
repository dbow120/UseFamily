#include <winsock2.h>
#include <stdio.h>
#include <time.h>

#define InitialType 1
#define TerminalType 2

#define PORT     8080
#define BUFFER_SIZE 1024

typedef struct {
    int Type;
    int seqNum;
    int length;
    char bytes[BUFFER_SIZE];
} dataPck;

typedef struct ackPck {
    int seqNum;
    int Type;
}ACK;

int alternateNum (int prevNum) {
   if (prevNum == 0) {
       return 1;
   }
    else {
        return 0;
    }
}

int main(int argc, char **argv)
{
     WSADATA            wsaData;
     SOCKET             ReceivingSocket;
     SOCKADDR_IN        ReceiverAddr, cliaddr;
     int                Port = 5150;
     SOCKADDR_IN        SenderAddr;
     int                SenderAddrSize = sizeof(SenderAddr);
     clock_t start, end;

    int len;
    dataPck Frame;
    ACK ackPacket;
	char* storedData;
    FILE *f1;

    printf("Start\n");

   // Initialize Winsock version 2.2
   if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
   {
        printf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
        return -1;
   }
   else
        printf("Server: The Winsock DLL status is %s.\n", wsaData.szSystemStatus);

     // Create a new socket to receive datagrams on.
     ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

     if (ReceivingSocket == INVALID_SOCKET)
     {
          printf("Server: Error at socket(): %ld\n", WSAGetLastError());
          // Clean up
          WSACleanup();
          // Exit with error
          return -1;
     }
     else
          printf("Server: socket() is OK!\n");

     // Set up a SOCKADDR_IN structure that will tell bind that we
     // want to receive datagrams from all interfaces using port 5150.

     // The IPv4 family
     ReceiverAddr.sin_family = AF_INET;
     // Port no. 5150
     ReceiverAddr.sin_port = htons(Port);
     // From all interface (0.0.0.0)
     ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   // Associate the address information with the socket using bind.
   // At this point you can receive datagrams on your bound socket.
   if (bind(ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
   {
        printf("Server: bind() failed! Error: %ld\n", WSAGetLastError());
        // Close the socket
        closesocket(ReceivingSocket);
        // Do the clean up
        WSACleanup();
        // and exit with error
        return -1;
     }
     else
        printf("Server: bind() is OK!\n");

        len = sizeof(cliaddr);  //len is value/resuslt
    storedData = (char*)malloc(BUFFER_SIZE);

    while(1) {
	recvfrom(ReceivingSocket, &Frame, sizeof(Frame),
    0,(struct sockaddr*) &cliaddr, &len);

    storedData = (char*)realloc(storedData, Frame.length);
    strncat(storedData, Frame.bytes, Frame.length);

    printf("Data Received\n");

    ackPacket.seqNum = alternateNum(Frame.seqNum);
    ackPacket.Type = Frame.Type;

    sendto(ReceivingSocket,&ackPacket, sizeof(ackPacket),
    0, (const struct sockaddr *) &cliaddr, len);

	if (Frame.Type == TerminalType) {
	printf("Final data received");
	break;
	}

    printf("Hello message sent.\n");
}

   // When your application is finished receiving datagrams close the socket.
   printf("Server: Finished receiving. Closing the listening socket...\n");
   if (closesocket(ReceivingSocket) != 0)
        printf("Server: closesocket() failed! Error code: %ld\n", WSAGetLastError());
   else
        printf("Server: closesocket() is OK\n");

   // When your application is finished call WSACleanup.
   printf("Server: Cleaning up...\n");
   if(WSACleanup() != 0)
        printf("Server: WSACleanup() failed! Error code: %ld\n", WSAGetLastError());
   else
        printf("Server: WSACleanup() is OK\n");
   // Back to the system
   return 0;
}

