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
#include "../libcs50/memory.h"
#include "../common/network.h"
//#include "display.h"
#include "../libcs50/set.h"
#include "../common/message.h"

/******** function declarations ********/
int game(char *guideId, char *team, char *player, char *host, int port);

/******** opCode handlers **********/
static void badOpCode(message_t *message, set_t *fieldAgents);
static void gameStatus(message_t *message, set_t *fieldAgents);
static void GSAgent(message_t *message, set_t *fieldAgents);
static void GSClue(message_t *message, set_t *fieldAgents);
static void GSSecret(message_t * message, set_t *fieldAgents);
static void GSResponse(message_t *message, set_t *fieldAgents);
static void teamRecord(message_t *message, set_t *fieldAgents);
static void gameOver(message_t *message, set_t *fieldAgents);

/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(message_t *message, set_t *fieldAgents);

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

/********* implementation *********/
int main(int argc, char **argv) 
{
	// temporary strings
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

	// parse arguments for their substrings
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


int game(char *guideId, char *team, char *player, char *host, int port)
{
	// try to connect to server, else return exit status > 0 to main
	connection_t *connection;
	if ((connection = openSocket(port, host)) == NULL) {
		return 5;
	}

	char *messagep;
	char *delim = "|";
	// open log directory and file to log activity
	FILE *log;
	if ((log = fopen("../logs/guideagent.log", "r")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 6;
	}

	set_t *fieldAgents = set_new();

	// loop runs until GAME_OVER message received, then breaks
	while (true) {

		messagep = receiveMessage(connection);

		if (messagep != NULL) {
			
			message_t *message = parseMessage(messagep);

			char *opCode = message->opCode;

			// loop over function table to call the correct opCode handler
			int fn;
			for (fn = 0; opCodes[fn].opCode != NULL; fn++) {
				if(strcmp(opCode, opCodes[fn].opCode) == 0) {
					(*opCodes[fn].func)(message, fieldAgents);
					break;
				}
			}

			if (opCodes[fn].opCode == NULL) {
				fprintf(stderr, "Unknown opCode: %s\n", opCode);
			}

			if (strcmp(opCode, "GAME_OVER") == 0) {
				break;
			}

		}

	}

	return 0;
}


// received an incorrect opCode, print error message and ignore
static void badOpCode(message_t *message, set_t *fieldAgents)
{
	fprintf(stderr, "incorrect opCode received, no actions performed\n");
}

// handle specific, applicable opCodes
static void gameStatus(message_t *message, set_t *fieldAgents)
{

}

static void GSAgent(message_t *message, set_t *fieldAgents) 
{

}

static void GSClue(message_t *message, set_t *fieldAgents)
{ 

}

static void GSSecret(message_t * message, set_t *fieldAgents)
{

}

static void GSResponse(message_t *message, set_t *fieldAgents) 
{

}

static void teamRecord(message_t *message, set_t *fieldAgents)
{

}

static void gameOver(message_t *message, set_t *fieldAgents)
{

}