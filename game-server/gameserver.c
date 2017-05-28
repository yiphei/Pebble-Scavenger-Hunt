/*
* server.c - Game server 
* Controls KRAG gameplay and allows players to connect
*
* Usage - ./gameserver gameId=... kff=... sf=... port=...
*
* mygcc gameserver.c ../common/network.h ../common/team.h ../common/message.h ../common/krag.h ../common/log.h ../libcs50/hashtable.h -o gameserver
*
* GREP, May 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <arpa/inet.h>        // socket-related calls
#include "../libcs50/hashtable.h"
#include "../common/team.h"
#include "../common/message.h"
#include "../common/log.h"
#include "../common/krag.h"



/******** globals *******/
bool gameInProgress;
int MAXOUTMESSAGELENGTH = 400; // used for mallocing out-messages

/******* functions *******/
int gameserver(char* gameId, char* kff, char* sf, int port);

// handler functions      
static void FAClaimHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
static void FALogHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
static void GAStatusHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
static void GAHintHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
static void FALocationHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);
static void badOpCodeHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);

// validate functions           
static bool validateMessageParse(char* gameId, message_t* message, connection_t*, FILE* log);
static int validateKrag(char* gameId, char* kragId, double latitude, double longitude, char* team, hashtable_t* teams, hashtable_t* krags);
static bool validateFA(char* gameId, message_t* message, hashtable_t* teams, hashtable_t* krags);
static bool validateGA(char* gameId, message_t* message, hashtable_t* teams, hashtable_t* krags);
static bool validatePebbleId(char* pebbleId, char* team, hashtable_t* teams);

// message sending functions
static bool sendGameStatus(char* gameId, char* guideId, int numClaimed, int numKrags, connection_t* connection, FILE* log);
static bool forwardHint(char* hintMessage, connection_t* connection, FILE* log);
static bool forwardHintToAll(char* hintMessage, char* team, char* teams, FILE* log);
static bool sendAllGSAgents(char* gameId, char* team, hashtable_t* teams, connection_t* connection, FILE* log);
static bool sendClue(char* gameId, char* guideId, char* clue, double latitude, double longitude, connection_t* connection, FILE* log);
static bool sendSecret(char* gameId, char* guideId, char* secret, connection_t* connection, FILE* log);
static bool sendGameOver(char* gameId, hashtable_t* teams, char* secret, FILE* log);
static bool sendTeamRecord(char* gameId, hashtable_t* teams, FILE* log);
static bool sendResponse(char* gameId, char* respCode, char* text, connection_t* connection, FILE* log);
static bool sendToAllFA(char* messagep, char* team, hashtable_t* teams);
static void sendToALLFAHelper(void* arg, const char* key, void* item);

/***** structs *****/
/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log);

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
    char *gameId = malloc(strlen(gameIdTemp) - 7);
	strcpy(gameId, gameIdTemp + 7);

    // invalid guideId length
    if (strlen(gameId) > 7 || strlen(gameId) == 0) {
        fprintf(stderr, "gameId should be 1-8 characters\n");
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
int gameserver(char* gameId, char* kff, char* sf, int port)
{
	// initialize variables
	connection_t* server; // server connection
	int socket; // socket
	hashtable_t* krags; // all krags
	hashtable_t* teams = initHash(); // keep track of the teams
	gameInProgress = true; // boolean for while loop
	char* secret;

	// open log file
	FILE* log;
	if((log = fopen("../logs/gameserver.log", "a")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 5;
	}


	// start the server
	server = startServer(port); // open server on correct port
	if(server == NULL){
		fprintf(stderr, "Unable to start server\n");
		return 6;
	}
	socket = server->socket;

	// load the krags
	krags = readKrag(kff);
	if(krags == NULL){
		fprintf(stderr, "Unable to load krag file\n");
		return 7;
	}

	// load the secret string
	secret = getSecretString(sf);

	// while the game hasn't ended
	while(gameInProgress){
		
		// initialize a return connection to receive from
		struct sockaddr rAddr;
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
		message_t* message = parseMessage(messageString);

		// check that message parsed correctly
		if(!validateMessageParse(gameId, message, rconn, log)){
			deleteConnection(rconn);
			continue; // skip to next iteration
		}

		// call function from function table to handle messages
		for (int fn = 0; opCodes[fn].opCode != NULL; fn++) {
			if(strcmp(message->opCode, opCodes[fn].opCode) == 0) {
				(*opCodes[fn].func)(gameId, messageString, message, teams, krags, rconn, log);
				free(messageString);
				deleteMessage(message);
				break;
			}
		}


		deleteConnection(rconn);
	}

	// end the game
	sendGameOver(gameId, teams, secret, log);
	sendTeamRecord(gameId, teams, log);

	// clean up
	deleteKragHash(krags); // delete the krag hashtable
	deleteTeamHash(teams); // delete the teams hashtable
	free(secret);
	fclose(log); // close the log file
	return 0; // return 0 for success
}

/*
* Handles an FA_CLAIM message
*
*
*/
static void FAClaimHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection); // log message
	
	// validate team and player
	if(!validateFA(gameId, message, teams, krags)){
		return;
	}

	// validate krag
	int valCode = validateKrag(gameId, message->kragId, message->latitude, message->longitude, message->team, teams, krags);
	if(valCode == 2){
		return; // ignore nonexistent krags
	} 
	else if(valCode == 1){
		// send claimed already response
		sendResponse(gameId, "SH_CLAIMED_ALREADY", message->kragId, connection, log);
	}
	else if(valCode == 0){
		// reveal the string if success and send claim message
		sendResponse(gameId, "SH_CLAIMED", message->kragId, connection, log);
		int revCode = revealCharacters(message->kragId, message->team, getRevealedString(message->team, teams), teams, krags);
		if(revCode == 1){
			// end the game but update the string
			gameInProgress = false;
			sendSecret(gameId, message->guideId, getRevealedString(message->team,teams), connection,log);
			return;
		}
		// send the updated message
		sendSecret(gameId, message->guideId, getRevealedString(message->team,teams), connection,log);

		krag_t* krag; // initialize the krag

		// add two clues to the team and send them
		for(int i = 0; i < 2; i++){
			// get random clue
			krag = randomClue(message->team, krags, teams);
			//sendClue
			sendClue(gameId, message->guideId, krag->clue, krag->latitude, krag->longitude, connection, log);
		}
	}
}

/*
* Handles an FA_LOG message
*
*
*/
static void FALogHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{
	logMessage(log, messagep, "FROM", connection); // log message
}

/*
* Handles a GA_STATUS message
*
*
*/
static void GAStatusHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection); // log message

	// validate the team and player
	if(!validateGA(gameId, message, teams, krags)){
		return;
	}

	// Add the guide agent if it doesn't exist
	// Return val is 1 if the agent already existed, 0 otherwise
	int returnVal = addGuideAgent(message->player, message->pebbleId, message->team, gameId, connection, teams);

	// if the player is new or if a status is requested
	if(returnVal == 0 || message->statusReq == 1){
		team_t* team = hashtable_find(teams, message->team);
		if(team == NULL){
			return;
		}
		// respond with GAME_STATUS
		if(!sendGameStatus(gameId, message->guideId, team->claimed, totalKrags(krags), connection,log)){
			return;
		}
	
		// send GS_AGENT messages
		if(!sendAllGSAgents(gameId, message->team, teams, connection, log)){
			return;
		}
	}

	
}

/*
* Handles a GA_HINT message
*
*
*/
static void GAHintHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection); // log message

	// validate message structure
	if(!validateGA(gameId, message, teams, krags)){
		return;
	}

	if(!validatePebbleId){
		return;
	}

	// forward to all field agents
	if(strcmp(message->pebbleId, "*") == 0){
		forwardHintToAll(messagep, messagep->team, teams);
	} else{
		// get the connection via pebble ID
		forwardHint(messagep, conn, log);
	}
}

/*
* Handles an FA_LOCATION message
*
*
*/
static void FALocationHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection); // log message

	// validate the Field Agent fields
	if(!validateFA(gameId, message, teams, krags)){
		return;
	}

	// add new field agent if it doesn't exist
	int returnVal = addFieldAgent(message->player, message->pebbleId, message->team, gameId, connection, teams);

	// update the field agent struct with the new location
	updateLocation(message->player, message->team, message->longitude, message->latitude, teams);

	// send GAME_STATUS (if requested)
	if(returnVal == 0 || message->statusReq == 1){
		// get the team
		team_t* team = hashtable_find(teams, message->team);
		if(team == NULL){
			return;
		}
		// respond with GAME_STATUS
		if(!sendGameStatus(gameId, message->guideId, team->claimed, totalKrags(krags), connection,log)){
			return;
		}
	}

}

/*
* Handles bad OP_CODE
*
*
*/
static void badOpCodeHandler(char* gameId, char *messagep, message_t *message, hashtable_t* teams, hashtable_t* krags, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection); // log message

	// send message
	if(!sendResponse(message->gameId, "SH_ERROR_INVALID_OPCODE", message->opCode, connection, log)){
		fprintf(stderr, "Unable to send badOpCode message\n");
		return;
	}

}

/*
* Checks if the message had an error when parsing
*
*
*/
static bool validateMessageParse(char* gameId, message_t* message, connection_t* connection, FILE* log){
	// check if there was an error parsing message
	if(message->errorCode == 0){
		return true;
	}
	else if(message->errorCode == 1){
		// send message
	if(!sendResponse(message->gameId, "SH_ERROR_DUPLICATE_FIELD", message->opCode, connection, log)){
		fprintf(stderr, "Unable to send duplicate field message\n");
	}
	}
	else if(message->errorCode == 2 || message->errorCode == 3){
		// send message
		if(!sendResponse(message->gameId, "SH_ERROR_INVALID_FIELD", message->opCode, connection, log)){
			fprintf(stderr, "Unable to send invalid field message\n");
		}
	}
	return false;
}

/*
* Validates located krag returning true on valid
* Return 0 if valid, 1 if found already, 2 if invalid
*
*/
static int validateKrag(char* gameId, char* kragId, double latitude, double longitude, char* team, hashtable_t* teams, hashtable_t* krags)
{
	// get longitude and latitude
	krag_t* krag = hashtable_find(krags, kragId);
	if(krag == NULL){
		return 2; // krag doesn't exist
	}
	double kragLatitude = (double)krag->latitude;
	double kragLongitude = (double)krag->longitude;

	// calculate differences
	double meters = 10;
	double latDiff = 0.0000089 * meters; // calculate 10 meters difference for lat
	double longDiff = latDiff/cos(latitude*0.018); // calculate 10 meters difference for long
	
	// create a box of 10 meter radius around the position
	double newLatPos = kragLatitude + latDiff; 
	double newLatNeg = kragLatitude - latDiff;
	double newLongPos = kragLongitude + longDiff;
	double newLongNeg = kragLongitude - longDiff;
	
	if(latitude <= newLatPos && latitude >= newLatNeg && longitude <= newLongPos && longitude >= newLongNeg){
		return addKrag(team, kragId, krags, teams); // add krag if not already found
	} else {
		return 2; // return invalid find
	}
}


/*
* Validates all fields for a Field agent
* Return true if valid
*
*/
static bool validateFA(char* gameId, message_t* message, hashtable_t* teams, hashtable_t* krags)
{
	// check for proper gameId
	if(gameId != message->gameId){
		return false;
	}
	// check that the team exists
	team_t* team = hashtable_find(teams, message->team);
	if(team == NULL){
		return false;
	}

	// check that the player exists
	fieldAgent_t* fa = set_find(team->FAset, message->player);
	if(fa == NULL){
		return false;
	}

	// check that the pebble Id is valid
	if(!validatePebbleId){
		return false;
	}

	return true; // return true for success
}

/*
* Validates all fields for a Guide Agent
* Return true if valid
*
*/
static bool validateGA(char* gameId, message_t* message, hashtable_t* teams, hashtable_t* krags)
{
	// check for proper gameId
	if(gameId != message->gameId){
		return false;
	}
	// check that the team exists
	team_t* team = hashtable_find(teams, message->team);
	if(team == NULL){
		return false;
	}

	// check that the player exists
	if(team->guideAgent == NULL){
		return false;
	}

	// check that the guideId is correct
	if(strcmp(team->guideAgent->guideID,message->guideId) != 0){
		return false;
	}

	return true; // return true for success
}

/*
* Validates the pebble ID
*
*
*/
static bool validatePebbleId(char* pebbleId, char* team, hashtable_t* teams)
{
	return true;
}

/*
* Sends a game status to a specified guide
* Returns true on sent
*
*/
static bool sendGameStatus(char* gameId, char* guideId, int numClaimed, int numKrags, connection_t* connection, FILE* log)
{
	// allocate enough space needed for the  message 
	char *messagep = malloc(sizeof(char) * MAXOUTMESSAGELENGTH);

	if (messagep == NULL) {
		return false;
	}

	// convert numClaimed to a string
	int numClaimedDigits = (int)((ceil(log10(numClaimed))+1)); // get the length of the string
	char* numClaimedStr= malloc(numClaimedDigits*sizeof(char));
	sprintf(numClaimedStr,"%d", numClaimed);



	// convert numKrags to a string
	int numKragsDigits = (int)((ceil(log10(numClaimed))+1)); // get the length of the string
	char* numKragsStr = malloc(numKragsDigits*sizeof(char));
	sprintf(numKragsStr, "%d", numKrags);



	// construct message inductively
	strcat(messagep, "opCode=GAME_STATUS|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|guideId=");
	strcat(messagep, guideId);
	strcat(messagep, "|numClaimed=");
	strcat(messagep, numClaimedStr);
	strcat(messagep, "|numKrags=");
	strcat(messagep, numKragsStr);

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

/*
* Forwards a hint to a specified field agent
* Returns true on sent
*
*/
static bool forwardHint(char* hintMessage, connection_t* connection, FILE* log)
{
	// send the message
	if (!sendMessage(hintMessage, connection)){
		return false;
	}

	// log the message
	logMessage(log, hintMessage, "TO", connection);

	return true;
}

/*
* Forwards a hint to all field agents
* Returns true on sent
*
*/
static bool forwardHintToAll(char* hintMessage, char* team, char* teams, FILE* log)
{
	// send the message
	sendToAllFA(hintMessage, team, teams);

	// log the message
	logMessage(log, hintMessage, "TO", connection);

	return true;
}

/*
* Sends all FA statuses to the GA
* Returns true on sent
*
*/
static bool sendAllGSAgents(char* gameId, char* team, hashtable_t* teams, connection_t* connection, FILE* log)
{
	return true;
}

/*
* Sends a clue to the GA
* Returns true on sent
*
*/
static bool sendClue(char* gameId, char* guideId, char* clue, double latitude, double longitude, connection_t* connection, FILE* log)
{
	// allocate enough space needed for the  message 
	char *messagep = malloc(sizeof(char) * MAXOUTMESSAGELENGTH);

	if (messagep == NULL) {
		return false;
	}

	// convert latitude to a string
	int latitudeDigits = (int)((ceil(log10(latitude))+1)); // get the length of the string
	char* latitudeStr = malloc(latitudeDigits*sizeof(char));
	sprintf(latitudeStr, "%d", latitude);

	// convert longitude to a string
	int longitudeDigits = (int)((ceil(log10(latitude))+1)); // get the length of the string
	char* longitudeStr = malloc(longitudeDigits*sizeof(char));
	sprintf(longitudeStr, "%d", longitude);

	// construct message inductively
	strcat(messagep, "opCode=GS_CLUE|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|guideId=");
	strcat(messagep, guideId);
	strcat(messagep, "|latitude=");
	strcat(messagep, latitudeStr);
	strcat(messagep, "|longitude=");
	strcat(messagep, longitudeStr);
	strcat(messagep, "|clue=");
	strcat(messagep, clue);

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

/*
* Sends the updated secret string to the GA
* Returns true on sent
*
*/
static bool sendSecret(char* gameId, char* guideId, char* secret, connection_t* connection, FILE* log)
{
	// allocate enough space needed for the  message 
	char *messagep = malloc(sizeof(char) * MAXOUTMESSAGELENGTH);

	if (messagep == NULL) {
		return false;
	}

	// construct message inductively
	strcat(messagep, "opCode=GS_SECRET|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|guideId=");
	strcat(messagep, guideId);
	strcat(messagep, "|secret=");
	strcat(messagep, secret);

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

/*
* Sends GAME_OVER to everyone
* Returns true on sent
*
*/
static bool sendGameOver(char* gameId, hashtable_t* teams, char* secret, FILE* log)
{
	// allocate enough space needed for the  message 
	char *messagep = malloc(sizeof(char) * MAXOUTMESSAGELENGTH);

	if (messagep == NULL) {
		return false;
	}

	// construct message inductively
	strcat(messagep, "opCode=GS_SECRET|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|secret=");
	strcat(messagep, secret);


	// send the message
	if (!sendMessage(messagep, connection)){
		return false;
	}

	// log the message
	logMessage(log, messagep, "TO", connection);

	// free the message
	free(messagep);

	return true;
 	return true;
}

/*
* Sends a record to every team
*
*
*/
static bool sendTeamRecord(char* gameId, hashtable_t* teams, FILE* log)
{
	return true;
}

/*
* Sends a response code and message
* Returns true on success
*
*/
static bool sendResponse(char* gameId, char* respCode, char* text, connection_t* connection, FILE* log)
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

/*
* Sends a message to every Field Agent on a team
*
*
*/
static bool sendToAllFA(char* messagep, char* team, hashtable_t* teams)
{	
	// get the set of field agents
	set_t* fieldAgents = getAllFieldAgents(team, teams);
	// iterate through the set sending the message
	set_iterate(fieldAgents, messagep, sendToALLFAHelper);
}


/*
* Iterator function to send to every Field Agent
*
*
*/
static void sendToALLFAHelper(void* arg, const char* key, void* item)
{
	char* message = (char*)arg; // cast arg
	fieldAgent_t* agent = (fieldAgent_t*)item; // cast item
	sendMessage(message, agent->conn); // send the message

}