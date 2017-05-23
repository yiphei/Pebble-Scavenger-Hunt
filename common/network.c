/*
* Network module for krag game
* Opens, closes, and otherwise handles sockets
*
* Tony DiPadova, May 2017
*
*/

#include "network.h"

/***** file-local variables *****/
static struct socketaddr_in remote;
static int BUFSIZE = 8192; // 8k 
/***** functions *****/

/****** openSocket ******/
bool openSocket(int port){
	// Create the socket
	int comm = socket(AF_INET, SOCK_DGRAM,0);
	if(comm < 0){
		fprintf(stderr , "Unable to create datagram socket\n");
		return false;
	}

	// initialize the socket address for the gameserver
	struct socketaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port); // bind to server specific IP

	// bind the socket
	if(bind(comm, (struct sockaddr *)&myaddr, sizeof(myaddr))<0){
		fprintf(stderr, "Unable to bind socket\n");
		return false;
	}

	return true; // return true for success
}

/***** receiveMessage *****/
char* receiveMessage()
{	// initialize variables
	unsigned char buf[BUFSIZE]; // character buffer to receive messages
	struct sockaddr_in remote; // remote address
	
	// receive messages
	int messageLen = recvfrom(comm, buf, BUFSIZE-1, 0, (struct sockaddr *)&remote, &remotelen);
	
	// if messages are received, return them
	if(messageLen > 0){
		char* messagep = malloc(messageLen); // allocate space for return pointer
		buf[messageLen] = '\0'; // add a null terminator to shorten the buffer
		strcpy(messagep, buf); // copy into a character pointer
		return messagep; // return the message
	}
	return NULL; // return NULL if no message received
}

/***** sendMessage *****/
bool sendMessage(char* message, sockaddr remote)
{
	sendto(comm, message, strlen(message), 0, remote, remotelen);
}

/***** closeSocket *****/
void closeSocket(void)
{
	close(comm); // close the socket
}