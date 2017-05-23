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
#include "../common/network.h"
#include "../libcs50/memory.h"
//#include "display.h"
#include "../libcs50/set.h"
#include "../common/message.h"
#include "../common/log.h"
#include "../common/team.h"

/******** function declarations ********/
int game(char *guideId, char *team, char *player, char *host, int port);
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, FILE *file);
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, FILE *file);

/******** opCode handlers **********/
static void badOpCode(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void gameStatus(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSAgent(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSClue(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSSecret(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void GSResponse(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void teamRecord(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);
static void gameOver(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log);

} opCodes[] = {
	{"FA_CLAIM", badOpCode},
	{"FA_LOG", badOpCode},
	{"GA_STATUS", badOpCode},
	{"GA_HINT", badOpCode},
	{"FA_LOCATION", badOpCode},
	{"GAME_STATUS", gameStatus},
	{"GS_AGENT", GSAgent},
	{"GS_CLUE", GSClue},
	{"GS_SECRET", GSSecret},
	{"GS_RESPONSE", GSResponse},
	{"TEAM_RECORD", teamRecord},
	{"GAME_OVER", gameOver},
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
	guideAgent_t *me = newGuideAgent(guideId, player);
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
	}

	char *gameId = me->gameId;

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

	}

	fclose(log);


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
static void badOpCode(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	fprintf(stderr, "incorrect opCode received, no actions performed\n");
	logMessage(log, messagep, "FROM", connection);
}

// handle specific, applicable opCodes
static void gameStatus(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{
	// first game status received, set gameId for later use
	if (teamp->guideAgent->gameId == "0") {
		teamp->guideAgent->gameId == message->gameId;
	}



	logMessage(log, messagep, "FROM", connection);
}

static void GSAgent(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection);
}

static void GSClue(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{ 

	logMessage(log, messagep, "FROM", connection);
}

static void GSSecret(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection);
}

static void GSResponse(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection);
}

static void teamRecord(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection);
}

static void gameOver(char *messagep, message_t *message, team_t *teamp, connection_t *connection, FILE *log)
{

	logMessage(log, messagep, "FROM", connection);
}