/*
* Network module for krag game
* Opens, closes, and otherwise handles sockets
* startServer must be called before listening, sending, or closing
* Tony DiPadova, May 2017
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



/*
* Function to start server
* Returns false on failure
*
*/
bool startServer(int serverPort, int proxyPort);

/*
* Listens for a message
* Returns the received message, or NULL if no message received
* Messages must be freed by the user
*
* Usage:
* 
* while(true){
*	char* message = receiveMessage();
*	if(message == NULL){
*		continue;
*	}
*   printf("%s\n",message);
*   free(message);
* }
*/
char* receiveMessage(void);

/*
* Sends a message
* Returns true on success, false on failure
*
*/
bool sendMessage(char* message);

/*
* Stops the server
*
*
*/
void stopServer(void);