/*
* Network module for krag game
* Opens, closes, and otherwise handles sockets
* startServer must be called before listening, sending, or closing
* GREP, May 2017
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>          // bool type
#include <unistd.h>           // read, write, close
#include <strings.h>          // bcopy, bzero
#include <arpa/inet.h>        // socket-related calls
#include <sys/wait.h>         // wait() and waitpid()
#include <netdb.h>

/***** structs *****/
typedef struct connection {
	int socket;
	struct sockaddr* address;
} connection_t;

/*
* Function to start server
* Returns connection
* Returns NULL if failure
*/
connection_t* startServer(int port);

/*
* Opens socket to server at host
* Returns connection type
* Returns NULL if failure
*/
connection_t* openSocket(int port, char* host);


/*
* Listens for a message
* Returns the received message, or NULL if no message received
* Messages must be freed by the user
* Takes connection type and updates address to last received address
*
* Usage:
* 
* while(true){
*	// better to save last address than do this
*	struct sockaddr* myaddr; // could be replaced by saved address
*	myaddr->sin_family = AF_INET;
*	connection_t connection = newConnection(0, myaddr);
*
*	char* message = receiveMessage(connection);
*	if(message == NULL){
*		continue;
*	}
*   printf("%s\n",message);
*	struct sockaddr* lastAddr = connection->address; // save last address
*	deleteConnection(connection); // not necessary, could reuse connection
*   free(message);
* }
*/
char* receiveMessage(connection_t* connection);

/*
* Sends a message
* Returns true on success, false on failure
* Takes connection type and sends to the address
*/
bool sendMessage(char* message, connection_t* connection);

/*
* Creates a new connection
* User is responsible for freeing it
*
*/
connection_t* newConnection(int socket, struct sockaddr* address);

/*
* Deletes a connection structure
*
*
*/
void deleteConnection(connection_t* connection);

/*
* Closes the socket
*
*
*/
void closeSocket(int comm);