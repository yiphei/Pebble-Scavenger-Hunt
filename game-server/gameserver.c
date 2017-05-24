/*
* server.c - Game server 
* Controls KRAG gameplay and allows players to connect
*
* Usage - ./gameserver gameId=... kff=... sf=... port=...
*
*
* Tony DiPadova, May 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "network.h"
#include "hashtable.h"
#include "team.h"
#include "message.h"
#include "log.h"
#include "krag.h"

/***** structs *****/
/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log);

} opCodes[] = {
	{"FA_CLAIM", FAClaimHandler},
	{"FA_LOG", FALogHandler},
	{"GA_STATUS", GAStatusHandler},
	{"GA_HINT", GAHintHandler},
	{"FA_LOCATION", FALocationHandler},
	{"GAME_STATUS", badOpCodeHandler},
	{"GS_AGENT", badOpCodeHandler},
	{"GS_CLUE", badOpCodeHandler},
	{"GS_SECRET", badOpCodeHandler},
	{"GS_RESPONSE", badOpCodeHandler},
	{"TEAM_RECORD", badOpCodeHandler},
	{"GAME_OVER", badOpCodeHandler},
	{NULL, NULL}
};


/******* functions *******/
int gameserver(int gameId, char* kff, char* sf, int port);

// handler functions
void FAClaimHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
void FALogHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
void GAStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
void GAHintHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
void FALocationHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

// validate functions
bool validateKrag(krag_t krag, hashtable_t krags);
bool validateFAClaim(messsage_t message, hashtable_t teams, hashtable_t krags);
bool validateFALog(messsage_t message, hashtable_t teams, hashtable_t krags);
bool validateGAStatus(messsage_t message, hashtable_t teams, hashtable_t krags);
bool validateGAHint(messsage_t message, hashtable_t teams, hashtable_t krags);
bool validateFALocation(messsage_t message, hashtable_t teams, hashtable_t krags);

// message sending functions
bool sendGameStatus(char* gameId, char* guideId, int numClaimed, int numKrags, connection_t* connection, FILE* log);
bool forwardHint(char* hintMessage, connection_t* connection, FILE* log);
bool sendAllGSAgents(char* gameId, char* team, hashtable_t* teams, connection_t* connection, FILE* log);
bool sendClue(char* gameId, char* guideId, char* clue, double latitude, double longitude, connection_t* connection, FILE* log);
bool sendSecret(char* gameId, char* guideId, char* secret, connection_t* connection, FILE* log);
bool sendGameOver(hashtable_t* teams, FILE* log);
bool sendResponse(char* gameId, char* respCode, char* text, connection_t* connection, FILE* log);


/*
* main function that parses arguments
* returns 0 on success, other codes otherwise
*
*/
int main(int argc, char* argv[])
{
	// Check number of arguments
	if(argc != 5){
		// Report error with number of parameters
		fprintf(stderr, "usage: ./gameserver gameId=... kff=... sf=... port=...\n");
		exit(1);
	}


	// temporary strings
    char* gameIdTemp = NULL;
    char* kffTemp = NULL;
    char* sfTemp = NULL;
    char* portTemp = NULL;

    // find and assign specific command line arguments
    for (int i = 1; i < argc; i++) {

        // compare portions of command line arguments to their names
        if (strncmp(argv[i], "gameId=", 6) == 0) {
            if (gameIdTemp == NULL){
                    gameIdTemp = argv[i];
            }

            else {
                    fprintf(stderr, "duplicate gameId argument\n");
                    exit(2);
            }
        }

        else if (strncmp(argv[i], "kff=", 3) == 0) {
            if (kffTemp == NULL) {
                    kffTemp = argv[i];
            }

            else { 
                    fprintf(stderr, "duplicate kff argument\n");
                    exit(2);
            }
        }

        else if (strncmp(argv[i], "sf=", 2) == 0) {
            if (sfTemp == NULL) {
                    sfTemp = argv[i];
            }

            else { 
                    fprintf(stderr, "duplicate sf argument\n");
                    exit(2);
            }
        }

        else if (strncmp(argv[i], "port=", 4) == 0) {
            if (portTemp == NULL) {
                    portTemp = argv[i];
            }

            else { 
                    fprintf(stderr, "duplicate port argument\n");
                    exit(2);
            }
        }

        else { 
            printf("warning: unrecognized argument %s\n", argv[i]); 
        }
    }

    // if any command line arguments were not supplied, exit
    if (gameIdTemp == NULL) {
        fprintf(stderr, "missing gameId=... argument\n");
        exit(3);
    }

    if (kffTemp == NULL) {
        fprintf(stderr, "missing kff=... argument\n");
        exit(3);
    }

    if (sfTemp == NULL) {
        fprintf(stderr, "missing sf=... argument\n");
        exit(3);
    }

    if (portTemp == NULL) {
        fprintf(stderr, "missing port=... argument\n");
        exit(3);
    }
    // parse arguments for their substrings
    char *gameIdTemp2 = malloc(strlen(gameIdTemp) - 7);
    gameIdTemp2 = strcpy(gameIdTemp2, gameIdTemp + 7);

    // invalid guideId length
    if (strlen(gameIdTemp2) > 7 || strlen(gameIdTemp2) == 0) {
        fprintf(stderr, "gameId should be 1-8 characters\n");
    }

    // invalid hexidecimal format
    unsigned int gameId;
    if (sscanf(gameIdTemp2, "%x", &gameId) != 1) {
        fprintf(stderr, "gameId is not in hexidecimal format\n");
        exit(4);
    }

    // assign kff
    char *kff = malloc(strlen(kffTemp) - 4);
    kff = strcpy(kff, kffTemp + 4);

    // assign sf
    char *sf = malloc(strlen(sfTemp) - 3);
    sf = strcpy(sf, sfTemp + 3);

    // assign port
    char *portTemp2 = malloc(strlen(portTemp) - 5);
    portTemp2 = strcpy(portTemp2, portTemp + 5);
    // check to see if port is an integer
    for (int i = 0; i < strlen(portTemp2); i++) {
        if (!isdigit(portTemp2[i])) {
                fprintf(stderr, "port is not an integer\n");
                exit(4);
        }
    }
    int port = atoi(portTemp2);



	// Run the server and store the exit code
	int result = gameserver(gameId, kff, sf, port);

	exit(result); // exit with the correct exit code
}


/*
* Game Server function that creates and runs the server
* Takes a game ID, krag file path, secret file path, and port number
*
*/
int gameserver(int gameId, char* kff, char* sf, int port)
{
	// initialize variables
	connection_t* server; // server connection
	int socket; // socket
	int numKrags; // number of krags
	hashtable_t krags; // all krags
	numTeams = 0; // keep track of the number of teams
	hashtable_t teams = initTeams(); // keep track of the teams
	bool gameInProgress = true; // boolean for while loop


	// open log file
	FILE* log;
	if if ((log = fopen("../logs/gameserver.log", "w")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 5;
	}


	// start the server
	server = startServer(36587); // open server on correct port
	if(server == NULL){
		fprintf(stderr, "Unable to start server\n");
		return 6;
	}
	socket = server->socket;

	// load the krags
	krags = readKrag(kff);
	if(krags == NULL){
		fprintf("Unable to load krag file\n");
		return 7;
	}
	numKrags = totalKrags(krags);

	// while the game hasn't ended
	while(gameInProgress){
		
		// initialize a return connection to receive from
		struct sockaddr* rAddr; // could be replaced by saved address
		rAddr->sin_family = AF_INET;
		connection_t* rconn = newConnection(socket, rAddr);
		if(rconn == NULL){
			continue; // skip message if connection is NULL
		}

		// receive and parse a message
		char* messageString = receiveMessage(rconn);
		if(messageString == NULL){
			deleteConnection(rconn);
			continue; // skip message if NULL
		}
		message_t message = parseMessage(messageString);

		// call function from function table to handle messages
		for (int fn = 0; opCodes[fn].opCode != NULL; fn++) {
			if(strcmp(opCode, opCodes[fn].opCode) == 0) {
				(*opCodes[fn].func)(messageString, message, teamp, connection, log);
				free(messageString);
				deleteMessage(message);
				break;
			}
		}


		deleteConnection(rconn);
	}


	// clean up
	deleteKragHash(krags); // delete the krag hashtable
	fclose(log); // close the log file
	return 0; // return 0 for success
}

/*
* Handles an FA_CLAIM message
*
*
*/
void FAClaimHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: validate krag id and location

	// TODO: update krag/team structs

	// TODO: send SH_CLAIMED or SH_CLAIMED_ALREADY

	// TODO: update team's secret string copy with GS_SECRET (end game if won)

	// TODO: Send two GS_CLUEs



	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles an FA_LOG message
*
*
*/
void FALogHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: log the text to the field agent log

	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles a GA_STATUS message
*
*
*/
void GAStatusHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: add the guide agent to the game

	// TODO: respond with GAME_STATUS (if requested)

	// TODO: send a GS_AGENT to GA for each FA (if requested)

	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles a GA_HINT message
*
*
*/
void GAHintHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: forward the hint to the appropriate field agent(s)

	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles an FA_LOCATION message
*
*
*/
void FALocationHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: add new field agent

	// TODO: update the field agent struct with the new location

	// TODO: send GAME_STATUS (if requested)

	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles bad OP_CODE
*
*
*/
void badOpCodeHandler(char *messagep, message_t *message, hashtable_t teams, hashtable_t krags, connection_t *connection, FILE *log)
{
	// TODO: validate message structure

	// TODO: send SH_ERROR_INVALID_OPCODE to caller

	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Validates located krag returning true on valid
* Return true if valid
*
*/
bool validateKrag(krag_t krag, hashtable_t krags)
{

}


/*
* Validates FA_CLAIM message
* Return true if valid
*
*/
bool validateFAClaim(messsage_t message, hashtable_t teams, hashtable_t krags)
{

}


/*
* Validates FA_LOG message
* Return true if valid
*
*/
bool validateFALog(messsage_t message, hashtable_t teams, hashtable_t krags)
{

}


/*
* Validates GA_STATUS message
* Return true if valid
*
*/
bool validateGAStatus(messsage_t message, hashtable_t teams, hashtable_t krags)
{

}


/*
* Validates GA_HINT message
* Return true if valid
*
*/
bool validateGAHint(messsage_t message, hashtable_t teams, hashtable_t krags)
{

}

/*
* Validates FA_LOCATION message
* Return true if valid
*
*/
bool validateFALocation(messsage_t message, hashtable_t teams, hashtable_t krags)
{

}

/*
* Sends a game status to a specified guide
* Returns true on sent
*
*/
bool sendGameStatus(char* gameId, char* guideId, int numClaimed, int numKrags, connection_t* connection, FILE* log)
{

}

/*
* Forwards a hint to a specified field agent
* Returns true on sent
*
*/
bool forwardHint(char* hintMessage, connection_t* connection, FILE* log)
{

}

/*
* Sends all FA statuses to the GA
* Returns true on sent
*
*/
bool sendAllGSAgents(char* gameId, char* team, hashtable_t* teams, connection_t* connection, FILE* log)
{

}

/*
* Sends a clue to the GA
* Returns true on sent
*
*/
bool sendClue(char* gameId, char* guideId, char* clue, double latitude, double longitude, connection_t* connection, FILE* log)
{

}

/*
* Sends the updated secret string to the GA
* Returns true on sent
*
*/
bool sendSecret(char* gameId, char* guideId, char* secret, connection_t* connection, FILE* log)
{

}

/*
* Sends GAME_OVER to everyone
* Returns true on sent
*
*/
bool sendGameOver(hashtable_t* teams, FILE* log)
{

}

/*
* Sends a response code and message
* Returns true on success
*
*/
bool sendResponse(char* gameId, char* respCode, char* text, connection_t* connection, FILE* log)
{
	// allocate full space needed for the  message (140+20 to account for unknown characters)
	char *messagep = malloc(strlen(gameId) + strlen(respCode) + 160);

	if (messagep == NULL) {
		return false;
	}

	// construct message inductively
	strcat(messagep, "opCode=GA_STATUS|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|respCode=");
	strcat(messagep, respCode);
	strcat(messagep, "|text=");
	strcat(messagep, text);

	// send the message
	if (!sendMessage(messagep, connection)){
		return false;
	}

	// log the message
	logMessage(log, messagep, "TO", connection);

	// free the message
	free(messagep);

	return true;
}