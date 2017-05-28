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
#include "guideagent.h"

/******** function declarations ********/
int game(char *guideId, char *team, char *player, char *host, int port);
int handleMessage(char *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, FILE *log, team_t *teamp);
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, FILE *file);
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, FILE *file);

/******** local opCode handlers **********/
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);
static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);

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
	void (*func)(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams);

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
	if ((log = fopen("../logs/guideagent.log", "w")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 6;
	}


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

	// init the team hashtable that will be used at GAME_OVER before game loop
	hashtable_t *teams = initHash();

	// initialize the interface
	initialize_curses();
	initializeWindows_I();

	// declare message types needed
	char *messagep;
	char *gameId;

	// loop runs until GAME_OVER message received, then breaks
	while (true) {

		// look for user input or message from Game Server
		messagep = receiveMessage(connection);

		char *hint = input_I();

		// handle the message if there is one, breaking if GAME_OVER received
		if (messagep != NULL) {
		
			int statusCheck = handleMessage(messagep, teamp, connection, log, teams);

			// GAME_OVER opCode received and handled
			if (statusCheck == 1) {
				break;
			}

			if (gameId == NULL) {
				gameId = me->gameID;
			}

		}

		// read for user input and send hint if there is input


		// if there was input, send it to the server
		if (hint != NULL) {

			handleHint(gameId, guideId, team, player, hint, connection, log, teamp);

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

	}


	fclose(log);
	deleteConnection(connection);
	deleteTeamHash(teams);

	return 0;
}

/*********** handleMessage *********/
int handleMessage(char *messagep, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{
	message_t *message = parseMessage(messagep);

	char *opCode = message->opCode;
	printf("%s\n", opCode);

	// loop over function table to call the correct opCode handler
	int fn;
	for (fn = 0; opCodes[fn].opCode != NULL; fn++) {
		if(strcmp(opCode, opCodes[fn].opCode) == 0) {
			(*opCodes[fn].func)(messagep, message, teamp, connection, log, teams);
			break;
		}
	}

	if (strcmp(opCode, "GAME_OVER") == 0) {
		free(messagep);
		return 1;
	}

	return 0;
}

/******** handleHint ********/
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, FILE *log, team_t *teamp)
{

	// allocate enough space for maximum length name (10 chars)
	char *name = malloc(11);

	sscanf(hint, "%s", name);

	NormalizeWord(name);

	fieldAgent_t *current = set_find(teamp->FAset, name);

	if (current != NULL) {

		sendGA_HINT(gameId, guideId, team, player, current->pebbleID, hint, connection, log);

	}

	else {

		sendGA_HINT(gameId, guideId, team, player, "*", hint, connection, log);

	}

	free(name);

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
// based on specific dispatch call, handle a message type accordingly

// received an incorrect opCode, print error message and ignore
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{
	fprintf(stderr, "incorrect opCode received, no actions performed\n");
	logMessage(log, messagep, "FROM", connection);
}

// handle specific, applicable opCodes
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{
	if (gameStatusValidate(message, teamp)) {

		// first game status received, set gameId for later use
		if (strcmp(teamp->guideAgent->gameID, "0") == 0) {
			teamp->guideAgent->gameID = message->gameId;
		}

		teamp->claimed = message->numClaimed;

		int totalKrags = message->numKrags;

		updateKragsClaimed_I(teamp->claimed);
		updateTotalKrags_I(totalKrags);

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
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

		updateMap_I(FAset);

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{ 
	if (GSClueValidate(message)) {

		// arbitrary key for the clue because it doesn't affect Guide Agent
		char *key = message->kragId;

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

static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{
	if (GSSecretValidate(message)) {

		// copy message's secret to be the team's secret
		strcpy(teamp->revealedString, message->secret);

		// update GUI with newly revealed secret
		updateString_I(teamp->revealedString);

		logMessage(log, messagep, "FROM", connection);

	}
}

static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{
	// validate GS respondse, if valid log it and do nothing else
	if (GSResponseValidate(message)) {
		logMessage(log, messagep, "FROM", connection);
	}
}

static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{

	// store each team's record, preparing to show their stats at GAME_OVER
	team_t *new = newTeam();

	new->claimed = message->numClaimed;
	new->numPlayers = message->numPlayers;

	hashtable_insert(teams, message->team, new);

	logMessage(log, messagep, "FROM", connection);
}

static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log, hashtable_t *teams)
{

	gameOver_I(teams);

	logMessage(log, messagep, "FROM", connection);
}

/********** message validation functions ************/
// validate each specific message type so it can be handled accordingly
static bool gameStatusValidate(message_t *message, team_t *teamp) 
{
	// gameId and guideId checks
	// never initialized
	if (message->guideId == NULL || message->gameId == NULL) {
		return false;
	} 

	// hexidecimal format check
	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		return false;
	}

	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSAgentValidate(message_t *message)
{
	// gameId and pebbleId checks
	// never initialized when parsing
	if (message->gameId == NULL || message->pebbleId == NULL) {
		return false;
	}

	// hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		return false;
	}

	unsigned int pebbleIdFormat;
	if (sscanf(message->pebbleId, "%x", &pebbleIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSClueValidate(message_t *message)
{
	if (message->gameId == NULL || message->guideId == NULL || message->clue == NULL) {
		return false;
	}

	// hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		return false;
	}

	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSSecretValidate(message_t *message)
{
	// message field initialization checks
	if (message->gameId == NULL || message->guideId == NULL || message->secret == NULL) {
		return false;
	}

	// hexidecimal format checks
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		return false;
	}

	unsigned int guideIdFormat;
	if (sscanf(message->guideId, "%x", &guideIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSResponseValidate(message_t *message)
{
	// uninitialized message fields check
	if (message->gameId == NULL || message->respCode == NULL || message->text == NULL) {
		return false;
	}

	// gameId hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		return false;
	}

	return true;
}
