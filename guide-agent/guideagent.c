/*
 * guide-agent.c - implements the KRAG game logic for the Guide Agent module
 * to handle input messages and update the interface based on input, and
 * send messages to server based on game status
 *
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>             // isdigit
#include <netdb.h>             // socket-related structures
#include <time.h>
#include <ncurses.h>
#include "display.h"
#include "guideagent.h"

/******** function declarations ********/
int game(char *guideId, char *team, char *player, char *host, int port);
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, FILE *file);
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, FILE *file);

/******** local opCode handlers **********/
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

/********* local message validation functions *********/
static bool gameStatusValidate(message_t *message, team_t *teamp);
static bool GSAgentValidate(message_t *message);
static bool GSClueValidate(message_t *message);
static bool GSSecretValidate(message_t *message);
static bool GSResponseValidate(message_t *message);

/********* file-local variables *********/

/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

} opCodes[] = {
	{"FA_CLAIM", badOpCodeHandler},
	{"FA_LOG", badOpCodeHandler},
	{"GA_STATUS", badOpCodeHandler},
	{"GA_HINT", badOpCodeHandler},
	{"FA_LOCATION", badOpCodeHandler},
	{"GAME_STATUS", gameStatusHandler},
	{"GS_AGENT", GSAgentHandler},
	{"GS_CLUE", GSClueHandler},
	{"GS_SECRET", GSSecretHandler},
	{"GS_RESPONSE", GSResponseHandler},
	{"TEAM_RECORD", teamRecordHandler},
	{"GAME_OVER", gameOverHandler},
	{NULL, NULL}
};

/********* function definitions *********/

/********* main ***********/
int main(int argc, char **argv) 
{
	// temporary strings that include the = statements in the arguments
	char *guideIdTemp = NULL;
	char *teamTemp = NULL;
	char *playerTemp = NULL;
	char *hostTemp = NULL;
	char *portTemp = NULL;

	// find and assign specific command line arguments
	for (int i = 1; i < argc; i++) {

		// compare portions of command line arguments to their names
		if (strncmp(argv[i], "guideId=", 7) == 0) {
			if (guideIdTemp == NULL){
				guideIdTemp = argv[i];
			}

			else {
				fprintf(stderr, "duplicate guideId argument\n");
				exit(2);
			}
		}

		else if (strncmp(argv[i], "team=", 4) == 0) {
			if (teamTemp == NULL) {
				teamTemp = argv[i];
			}

			else { 
				fprintf(stderr, "duplicate team argument\n");
				exit(2);
			}
		}

		else if (strncmp(argv[i], "player=", 6) == 0) {
			if (playerTemp == NULL) {
				playerTemp = argv[i];
			}

			else { 
				fprintf(stderr, "duplicate player argument\n");
				exit(2);
			}
		}

		else if (strncmp(argv[i], "host=", 4) == 0) {
			if (hostTemp == NULL) {
				hostTemp = argv[i];
			}

			else { 
				fprintf(stderr, "duplicate host argument\n");
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
	if (guideIdTemp == NULL) {
		fprintf(stderr, "missing guideId=... argument\n");
		exit(3);
	}

	if (teamTemp == NULL) {
		fprintf(stderr, "missing team=... argument\n");
		exit(3);
	}

	if (playerTemp == NULL) {
		fprintf(stderr, "missing player=... argument\n");
		exit(3);
	}

	if (hostTemp == NULL) {
		fprintf(stderr, "missing host=... argument\n");
		exit(3);
	}

	if (portTemp == NULL) {
		fprintf(stderr, "missing port=... argument\n");
		exit(3);
	}

	// parse arguments for their substrings (past the = statements)
	char *guideId = malloc(strlen(guideIdTemp) - 8);
	guideId = strcpy(guideId, guideIdTemp + 8);

	// invalid guideId length
	if (strlen(guideId) > 8 || strlen(guideId) == 0) {
		fprintf(stderr, "guideId should be 1-8 characters\n");
	}

	// invalid hexidecimal format
	unsigned int guideIdFormat;
	if (sscanf(guideId, "%x", &guideIdFormat) != 1) {
		fprintf(stderr, "guideId is not in hexidecimal format\n");
		exit(4);
	}

	char *team = malloc(strlen(teamTemp) - 5);
	team = strcpy(team, teamTemp + 5);
	// team name exceeds max length
	if (strlen(team) > 10) {
		fprintf(stderr, "max team name length is 10 characters\n");
		exit(4);
	}

	char *player = malloc(strlen(playerTemp) - 7);
	player = strcpy(player, playerTemp + 7);
	// player name exceeds max length
	if (strlen(player) > 10) {
		fprintf(stderr, "max player name length is 10 characters\n");
		exit(4);
	}

	char *host = malloc(strlen(hostTemp) - 5);
	host = strcpy(host, hostTemp + 5);

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
	free(portTemp2);

	int exitStatus = game(guideId, team, player, host, port);

	// free the original parameters
	free(guideId);
	free(team);
	free(player);
	free(host);

	exit(exitStatus);
}

/*********** game ***********/
int game(char *guideId, char *team, char *player, char *host, int port)
{
	// try to connect to server, else return exit status > 0 to main
	connection_t *connection;
	if ((connection = openSocket(port, host)) == NULL) {
		return 5;
	}

	// open log directory and file to log activity
	FILE *log;
	if ((log = fopen("../logs/guideagent.log", "r")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 6;
	}

	char *messagep;
	message_t *message;
	char *opCode;

	time_t start = time(NULL);
	time_t now = time(NULL);
	time_t elapsedSeconds = now - start;

	// send Game Server first GA_STATUS
	sendGA_STATUS("0", guideId, team, player, "1", connection, log);

	// make a new team
	team_t *teamp = newTeam();

	if (teamp == NULL) {
		fprintf(stderr, "error allocating memory\n");
		return 7;
	}

	// add this agent to the team and initializes its values
	guideAgent_t *me = newGuideAgent(guideId, player, "0", connection);
	teamp->guideAgent = me;

	/* wait until server sends GAME_STATUS back to start game loop.
	Once received, initialize the team struct and break this loop */
	while (true) {
		messagep = receiveMessage(connection);
		message = parseMessage(messagep);
		opCode = message->opCode;

		// initialize this Guide Agent's team
		for (int fn = 0; opCodes[fn].opCode != NULL; fn++) {
			if(strcmp(opCode, opCodes[fn].opCode) == 0) {
				(*opCodes[fn].func)(messagep, message, teamp, connection, log);
				free(messagep);
				deleteMessage(message);
				break;
			}
		}

		free(messagep);
		deleteMessage(message);
	}

	char *gameId = me->gameID;

	// TODO: create and initialize interface

	// loop runs until GAME_OVER message received, then breaks
	while (true) {

		messagep = receiveMessage(connection);

		if (messagep != NULL) {
			
			message = parseMessage(messagep);

			opCode = message->opCode;

			// loop over function table to call the correct opCode handler
			int fn;
			for (fn = 0; opCodes[fn].opCode != NULL; fn++) {
				if(strcmp(opCode, opCodes[fn].opCode) == 0) {
					(*opCodes[fn].func)(messagep, message, teamp, connection, log);
					break;
				}
			}

			if (strcmp(opCode, "GAME_OVER") == 0) {
				break;
			}

			if (opCodes[fn].opCode == NULL) {
				fprintf(stderr, "Unknown opCode: %s\n", opCode);
			}

			// free pointers for next time around the loop
			free(messagep);
			deleteMessage(message);

		}

		// every 30 seconds send a GA_STATUS
		now = time(NULL);
		elapsedSeconds = now - start;
		if ((elapsedSeconds) % 30 == 0) {

			char * statusReq = "0";

			// every minute ask for a game status update
			if ((elapsedSeconds) % 60 == 0) {
				statusReq = "1";
			}

			// try to send message to Game Server
			if(!sendGA_STATUS(gameId, guideId, team, player, statusReq, connection, log)){
				fprintf(stderr, "could not send GA_STATUS to Game Server\n");
			}

		}

		free(messagep);
		deleteMessage(message);
	}


	fclose(log);
	deleteConnection(connection);
	deleteTeam(teamp);

	return 0;
}


/*********** sendGA_STATUS ************/
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, FILE *file)
{
	// allocate full space needed for the  message (60 being known characters)
	char *messagep = malloc(strlen(gameId) + strlen(guideId) + strlen(team)
		+strlen(player) + 60);

	if (messagep == NULL) {
		return false;
	}

	// construct message inductively
	strcat(messagep, "opCode=GA_STATUS|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|guideId=");
	strcat(messagep, guideId);
	strcat(messagep, "|team=");
	strcat(messagep, team);
	strcat(messagep, "|player=");
	strcat(messagep, player);
	strcat(messagep, "|statusReq=");
	strcat(messagep, statusReq);


	if (!sendMessage(messagep, connection)) {
		return false;
	}

	logMessage(file, messagep, "TO", connection);

	free(messagep);

	return true;
}

/********** sendGA_Hint **********/
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, FILE *file)
{
	char *messagep = malloc(strlen(gameId) + strlen(guideId) + strlen(team) 
		+ strlen(player) + strlen(pebbleId) + strlen(hint) + 64);

	if (messagep == NULL) {
		return false;
	} 

	// construct message inductively
	strcat(messagep, "opCode=GA_STATUS|gameId=");
	strcat(messagep, gameId);
	strcat(messagep, "|guideId=");
	strcat(messagep, guideId);
	strcat(messagep, "|team=");
	strcat(messagep, team);
	strcat(messagep, "|player=");
	strcat(messagep, player);
	strcat(messagep, "|pebbleId=");
	strcat(messagep, pebbleId);
	strcat(messagep, "|hint=");
	strcat(messagep, hint);

	if (!sendMessage(messagep, connection)) {
		return false;
	}

	logMessage(file, messagep, "TO", connection);

	free(messagep);

	return true;
}

/*********** dispath/opCode functions **********/
// received an incorrect opCode, print error message and ignore
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	fprintf(stderr, "incorrect opCode received, no actions performed\n");
	logMessage(log, messagep, "FROM", connection);
}

// handle specific, applicable opCodes
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	if (gameStatusValidate(message, teamp)) {

		// first game status received, set gameId for later use
		if (strcmp(teamp->guideAgent->gameID, "0") == 0) {
			teamp->guideAgent->gameID = message->gameId;
		}

		teamp->claimed = message->numClaimed;

		int totalKrags = message->numKrags;

		// TODO: update the interface

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	if (GSAgentValidate(message)) {

		char *player = message->player;
		double latitude = message->latitude;
		double longitude = message->longitude;

		set_t *FAset = teamp->FAset;

		fieldAgent_t *FA = set_find(FAset, player);

		// if the field agent is already in the game, just update position
		if (FA != NULL) {
			FA->latitude = latitude;
			FA->longitude = longitude;
		}

		// else, add him to the set and set his positioning
		else {
			FA = newFieldAgent(message->gameId, message->pebbleId, NULL);
			FA->latitude = latitude;
			FA->longitude = longitude;

			set_insert(FAset, player, FA);
		}

		// TODO: update interface based on new locations

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{ 
	if (GSClueValidate(message)) {

		// arbitrary key for the clue because it doesn't affect Guide Agent
		int keyNum = rand();

		// allocate 6 because rand only gives int of 5 digits long maximum
		char *key = malloc(6);
		sprintf(key, "%d", keyNum);

		// add the clue to the set
		set_t *clues = teamp->clues;

		set_insert(clues, key, message->clue);

		// update GUI with new clue
		updateClues_I(clues);

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	if (GSSecretValidate(message)) {

		// copy message's secret to be the team's secret
		strcpy(teamp->revealedString, message->secret);

		// update GUI with newly revealed secret
		updateString_I(teamp->revealedString);

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	// validate GS respondse, if valid log it and do nothing else
	if (GSResponseValidate(message)) {
		logMessage(log, messagep, "FROM", connection);
	}
}

static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	// store claimed krag number, preparing to show it at GAME_OVER
	teamp->claimed = message->numClaimed;

	logMessage(log, messagep, "FROM", connection);
}

static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	// TODO: update GUI to tell user game is over with final game stats and
	// show the fully revealed secret

	logMessage(log, messagep, "FROM", connection);
}

/********** message validation functions ************/
static bool gameStatusValidate(message_t *message, team_t *teamp) 
{
	// gameId and guideId checks
	// never initialized
	if (message->guideId == NULL || message->gameId == NULL) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	} 

	// hexidecimal format check
	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	// numClaimed field was not initialized when parsing
	if (message->numClaimed == 0 && teamp->claimed != 0) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	// numKrags field was not initialized when parsing (can't be 0 total krags)
	if (message->numKrags == 0) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	return true;
}

static bool GSAgentValidate(message_t *message)
{
	// gameId and pebbleId checks
	// never initialized when parsing
	if (message->gameId == NULL || message->pebbleId == NULL) {
		fprintf(stderr, "invalid GS_AGENT message\n");
		return false;
	}

	// hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	unsigned int pebbleIdFormat;
	if (sscanf(message->pebbleId, "%x", &pebbleIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	// longitude and latitude initialization checks
	if (message->latitude == 0) {
		fprintf(stderr, "invalid GS_AGENT message\n");
		return false;
	}

	if (message->longitude == 0) {
		fprintf(stderr, "invalid GS_AGENT message\n");
		return false;
	}

	return true;
}

static bool GSClueValidate(message_t *message)
{
	if (message->gameId == NULL || message->guideId == NULL || message->clue == NULL) {
		fprintf(stderr, "invalid GS_CLUE message\n");
		return false;
	}

	// hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	return true;
}

static bool GSSecretValidate(message_t *message)
{
	// message field initialization checks
	if (message->gameId == NULL || message->guideId == NULL || message->secret == NULL) {
		fprintf(stderr, "invalid GS_SECRET message\n");
		return false;
	}

	// hexidecimal format checks
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	return true;
}

static bool GSResponseValidate(message_t *message)
{
	// uninitialized message fields check
	if (message->gameId == NULL || message->respCode == NULL || message->text == NULL) {
		fprintf(stderr, "invalid GS_RESPONSE message\n");
		return false;
	}

	// gameId hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		fprintf(stderr, "invalid GAME_STATUS message\n");
		return false;
	}

	return true;
}
