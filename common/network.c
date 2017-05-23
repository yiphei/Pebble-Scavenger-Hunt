/*
* Network module for krag game
* Opens, closes, and otherwise handles sockets
*
* Tony DiPadova, May 2017
*
*/

#include "network.h"

/***** file-local variables *****/
static struct socketaddr_in gameserver;
static struct socketaddr_in proxy;
static int BUFSIZE = 8192; // 8k 
static int comm; // the socket
static socklen_t proxylen; // size of proxy

/***** functions *****/

/****** openSocket ******/
bool openSocket(int serverPort, int proxyPort){

	// Create the socket
	comm = socket(AF_INET, SOCK_DGRAM,0);
	if(comm < 0){
		fprintf(stderr , "Unable to create datagram socket\n");
		return false;
	}

	// initialize the socket address for the gameserver
	gameserver.sin_family = AF_INET;
	gameserver.sin_addr.s_addr = htonl(INADDR_ANY);
	gameserver.sin_port = htons(serverPort); // bind to server specific IP

	// bind the socket
	if(bind(comm, (struct sockaddr *)gameserver, sizeof(gameserver))<0){
		fprintf(stderr, "Unable to bind socket\n");
		return false;
	}
	
	// initialize the socket address for the proxy
	proxy.sin_family = AF_INET;
	proxy.sin_addr.s_addr = htonl(INADDR_ANY);
	proxy.sin_port = htons(proxyPort); // bind to proxy specific IP

	// initialize proxy size
	proxylen = sizeof(proxy);

	return true; // return true for success
}

/***** receiveMessage *****/
char* receiveMessage(void)
{
	unsigned char buf[BUFSIZE]; // character buffer to receive messages
	
	// receive messages
	int messageLen = recvfrom(comm, buf, BUFSIZE, 0, (struct sockaddr *)&proxy, &proxylen);
	
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
bool sendMessage(char* message)
{
	sendto(comm, message, strlen(message), 0, (struct sockaddr *)&proxy, proxylen);
}

/***** closeSocket *****/
void closeSocket(void)
{
	close(comm); // close the socket
}