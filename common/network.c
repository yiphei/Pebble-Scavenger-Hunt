/*
* Network module for krag game
* Opens, closes, and otherwise handles sockets
*
* GREP, May 2017
*
* Code used from client-server-udp examples
*/

#ifdef NOPEBBLE
#include "network.h"

/***** local global variables *****/
static int BUFSIZE = 8192; // 8k 
/***** functions *****/

/****** openSocket ******/
connection_t* startServer(int port){
	// Create the socket
	int comm = socket(AF_INET, SOCK_DGRAM,0);
	if(comm < 0){
		fprintf(stderr , "Network: Unable to create datagram socket\n");
		return NULL;
	}

	// initialize the socket address for the gameserver
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port); // bind to server specific IP

	// bind the socket
	if(bind(comm, (struct sockaddr *)&myaddr, sizeof(myaddr))<0){
		fprintf(stderr, "Network: Unable to bind socket\n");
		return NULL;
	}

	struct sockaddr* newaddrp = (struct sockaddr *) &myaddr;
	// create connection to return
	connection_t* connection = newConnection(comm, *newaddrp);
	return connection; 
}

connection_t* openSocket(int port, char* host){
	// check parameter
	if(host == NULL){
		return NULL;
	}
	// connect to host
	struct hostent *hostp = gethostbyname(host);

	// check for valid host/successful host search
	if (hostp == NULL) {
		fprintf(stderr, "Network: unknown host name: %s\n", host);
		return NULL;
	}

	// Initialize fields of the server address
	struct sockaddr_in server;  // address of the server
	server.sin_family = AF_INET;
	bcopy(hostp->h_addr_list[0], &server.sin_addr, hostp->h_length);
	server.sin_port = htons(port);

	  // Create socket
  	int comm = socket(AF_INET, SOCK_DGRAM, 0);
  	if (comm < 0) {
    	fprintf(stderr,"Network: Unable to open socket\n");
    	return NULL;
  	}
  	struct sockaddr* newaddrp = (struct sockaddr *) &server;
  	// create connection to return
  	connection_t* connection = newConnection(comm, *newaddrp);
  	return connection;
}

/***** receiveMessage *****/
char* receiveMessage(connection_t* connection)
{	// initialize variables
	unsigned char buf[BUFSIZE]; // character buffer to receive messages
	
	// check connection
	if(connection == NULL){
		return NULL;
	}
	// get remote
	struct sockaddr remote = connection->address;

	// get struct variables
	socklen_t remoteLen = sizeof(remote);
	int comm = connection->socket;

	// receive messages
	int messageLen = recvfrom(comm, buf, BUFSIZE-1, 0, &(connection->address), &remoteLen);
	
	// if messages are received, return them
	if(messageLen > 0){
		char* messagep = calloc(messageLen+1,sizeof(char)); // allocate space for return pointer
		buf[messageLen] = '\0'; // add a null terminator to shorten the buffer
		strcpy(messagep, (char *)buf); // copy into a character pointer
		return messagep; // return the message
	}
	return NULL; // return NULL if no message received
}

/***** sendMessage *****/
bool sendMessage(char* message, connection_t* connection)
{
	// check for null parameters
	if(connection == NULL || message == NULL){
		fprintf(stderr, "Network: Unable to send message, connection or message is NULL\n");
		return false;
	}

	// get the address and socket
	struct sockaddr remote = connection->address;
	socklen_t remoteLen = sizeof(remote);
	int comm = connection->socket;

	// send message
	if(sendto(comm, message, strlen(message), 0, &(connection->address), remoteLen) < 0){
		fprintf(stderr, "Network: Unable to send message: %s\n",message);
		return false;
	}
	return true;
}

/***** newConnection *****/
connection_t* newConnection(int socket, struct sockaddr address)
{
	// allocate space
	connection_t* conn = malloc(sizeof(connection_t));
	if(conn == NULL){
		fprintf(stderr, "Network: Unable to create connection struct\n");
		return NULL;
	}
	conn->socket = socket;
	conn->address = address;
	return conn;
}

/***** deleteConnection *****/
void deleteConnection(connection_t* connection)
{
	free(connection);
}

/***** closeSocket *****/
void closeSocket(int comm)
{
	close(comm); // close the socket
}

#endif