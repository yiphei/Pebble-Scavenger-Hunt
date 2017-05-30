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
#include <arpa/inet.h>        // socket-related calls
#include <sys/select.h>       // select-related stuff 
#include <ncurses.h>
#include "guideagent.h"

/******** function declarations ********/
int game(char *guideId, char *team, char *player, char *host, int port);
int handleMessage(char *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, char *filePath, team_t *teamp);
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, char *filePath);
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, char *filePath);

/******** local opCode handlers **********/
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void GSClaimedHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);
static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);

/********* local message validation functions *********/
static bool gameStatusValidate(message_t *message, team_t *teamp);
static bool GSAgentValidate(message_t *message);
static bool GSClueValidate(message_t *message);
static bool GSClaimedValidate(message_t *message);
static bool GSSecretValidate(message_t *message);
static bool GSResponseValidate(message_t *message);

/********* functions dispatch table *********/
static const struct {

	const char *opCode;
	void (*func)(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);

} opCodes[] = {
	{"FA_CLAIM", badOpCodeHandler},
	{"FA_LOG", badOpCodeHandler},
	{"GA_STATUS", badOpCodeHandler},
	{"GA_HINT", badOpCodeHandler},
	{"FA_LOCATION", badOpCodeHandler},
	{"GAME_STATUS", gameStatusHandler},
	{"GS_AGENT", GSAgentHandler},
	{"GS_CLUE", GSClueHandler},
	{"GS_CLAIMED", GSClaimedHandler},
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
	char *guideId = calloc(strlen(guideIdTemp) - 7, 1);
	strcpy(guideId, guideIdTemp + 8);
	strcat(guideId, "\0");

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

	char *team = calloc(strlen(teamTemp) - 4, 1);
	team = strcpy(team, teamTemp + 5);
	strcat(team, "\0");

	// team name exceeds max length
	if (strlen(team) > 10) {
		fprintf(stderr, "max team name length is 10 characters\n");
		exit(4);
	}

	char *player = calloc(strlen(playerTemp) - 6, 1);
	player = strcpy(player, playerTemp + 7);
	strcat(player, "\0");

	// player name exceeds max length
	if (strlen(player) > 10) {
		fprintf(stderr, "max player name length is 10 characters\n");
		exit(4);
	}

	char *host = calloc(strlen(hostTemp) - 4, 1);
	host = strcpy(host, hostTemp + 5);
	strcat(host, "\0");

	char *portTemp2 = calloc(strlen(portTemp) - 4, 1);
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
	portTemp2 = NULL;

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
	char *filePath = "../logs/guideagent.log";
	if ((log = fopen(filePath, "w")) == NULL) {
		fprintf(stderr, "error opening log file\n");
		return 6;
	}

	fclose(log);

	// send Game Server first GA_STATUS
	sendGA_STATUS("0", guideId, team, player, "1", connection, filePath);

	// make a new team
	team_t *teamp = newTeam();


	if (teamp == NULL) {
		fprintf(stderr, "error allocating memory for team\n");
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
	char *messagep = NULL;
	char *gameId = NULL;
	int statusReq = 0;

	// loop runs until GAME_OVER message received, then breaks
	while (true) {

		/**** from chatclient2.c in udp-select *****/
		// for use with select()
	    fd_set rfds;              // set of file descriptors we want to read
	    
	    // Watch stdin (fd 0) and the UDP socket to see when either has input.
		int socket = connection->socket;

	    FD_ZERO(&rfds);
	    FD_SET(0, &rfds);         // stdin
	    FD_SET(socket, &rfds); // the UDP socket
	    int nfds = socket+1;   // highest-numbered fd in rfds
	    /****** end source code ********/

		int select_response = select(nfds, &rfds, NULL, NULL, NULL);

		// input from one of two sources found
		if (select_response > 0) {

			// user input handling
			if (FD_ISSET(0, &rfds)) {

				char *hint = input_I();

				if (hint != NULL) {
					handleHint(gameId, guideId, team, player, hint, connection, filePath, teamp);
				}

				free(hint);
				hint = NULL;

			}

			// message handling
			if (FD_ISSET(socket, &rfds)) {
				messagep = receiveMessage(connection);

				// handle the message if there is one, breaking if GAME_OVER received
				if (messagep != NULL) {
				
					// int checks if game is over or not
					int statusCheck = handleMessage(messagep, teamp, connection, filePath, teams);

					// GAME_OVER opCode received and handled
					if (statusCheck == 1) {
						break;
					}

					// assign gameId when first GAME_STATUS is received for later use
					if (gameId == NULL) {
						gameId = teamp->guideAgent->gameID;
					}

					// keep track of when to send GA_STATUS (every 3 messages received)
					if (statusReq == 3) {
						sendGA_STATUS(gameId, guideId, team, player, "0", connection, filePath);
						statusReq++;
					}

					else if (statusReq == 6) {
						sendGA_STATUS(gameId, guideId, team, player, "1", connection, filePath);
						statusReq = 0;
					}

					else {
						statusReq++;
					}

					messagep = NULL;

				}
			
			}

		}

	}

	hashtable_insert(teams, "this", teamp);

	deleteConnection(connection);
	deleteTeamHashGA(teams);

	return 0;
}

/*********** handleMessage *********/
int handleMessage(char *messagep, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	message_t *message = parseMessage(messagep);

	if (message->errorCode == 0) {
		// loop over function table to call the correct opCode handler
		int fn;
		for (fn = 0; opCodes[fn].opCode != NULL; fn++) {
			if(strcmp(message->opCode, opCodes[fn].opCode) == 0) {
				(*opCodes[fn].func)(messagep, message, teamp, connection, filePath, teams);
				break;
			}
		}

		free(messagep);
		messagep = NULL;

		if (strcmp(message->opCode, "GAME_OVER") == 0) {
			deleteMessage(message);
			message = NULL;
			return 1;
		}

	}

	deleteMessage(message);
	message = NULL;

	return 0;
}

/******** handleHint ********/
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, char *filePath, team_t *teamp)
{
	if (gameId != NULL) {
		char *name = calloc(140, 1);

		sscanf(hint, "%s", name);

		NormalizeWord(name);

		for (int i = 0; ; i++) {
			if (!isalpha(name[i])) {
				name[i] = '\0';
				break;
			}
		}

		fieldAgent_t *current = set_find(teamp->FAset, name);

		if (current != NULL) {

			sendGA_HINT(gameId, guideId, team, player, current->pebbleID, hint, connection, filePath);

		}

		else {

			sendGA_HINT(gameId, guideId, team, player, "*", hint, connection, filePath);

		}

		free(name);
		name = NULL;
	}

}

/*********** sendGA_STATUS ************/
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, char *filePath)
{
	// allocate 8k buffer to the message
	char *messagep = calloc(400, 1);

	if (messagep == NULL) {
		return false;
	}

	// construct message inductively
	strcpy(messagep, "opCode=GA_STATUS|gameId=");
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

	logMessage(filePath, messagep, "TO", connection);

	free(messagep);
	messagep = NULL;

	return true;
}

/********** sendGA_Hint **********/
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, char *filePath)
{
	char *messagep = calloc(400, 1);

	if (messagep == NULL) {
		return false;
	} 

	// construct message inductively
	strcpy(messagep, "opCode=GA_HINT|gameId=");
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

	logMessage(filePath, messagep, "TO", connection);

	free(messagep);
	messagep = NULL;

	return true;
}

/*********** dispath/opCode functions **********/
// based on specific dispatch call, handle a message type accordingly

// received an incorrect opCode, print error message and ignore
static void badOpCodeHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	logMessage(filePath, messagep, "FROM", connection);
}

// handle specific, applicable opCodes
static void gameStatusHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	if (gameStatusValidate(message, teamp)) {

		// first game status received
		if (strcmp(teamp->guideAgent->gameID, "0") == 0) {
			char *gameId = calloc(strlen(message->gameId) + 1, 1);

			strcpy(gameId, message->gameId);

			free(teamp->guideAgent->gameID);

			teamp->guideAgent->gameID = calloc(strlen(gameId) + 1, 1);

			strcpy(teamp->guideAgent->gameID, gameId);
			strcat(teamp->guideAgent->gameID, "\0");

			free(gameId);
		}

		// update the GUI with new (or not new) info
		teamp->claimed = message->numClaimed;

		int totalKrags = message->numKrags;

		updateKragsClaimed_I(teamp->claimed);
		updateTotalKrags_I(totalKrags);

		logMessage(filePath, messagep, "FROM", connection);

	}
}

static void GSAgentHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	if (GSAgentValidate(message)) {

		double latitude = message->latitude;
		double longitude = message->longitude;

		fieldAgent_t *FA = set_find(teamp->FAset, message->player);

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

			NormalizeWord(message->player);

			set_insert(teamp->FAset, message->player, FA);
		}

		updateMap_I(teamp->FAset, teamp->krags);

		logMessage(filePath, messagep, "FROM", connection);

	}

	else {
		fprintf(stderr, "GSAgent not valid\n");
	}
}

static void GSClueHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{ 
	if (GSClueValidate(message)) {

		// only add clue if krag hasn't been found (error check)
		if (set_find(teamp->krags, message->kragId) == NULL) {
			// add the clue to the set

			char *clue = calloc(strlen(message->clue) + 2, 1);
			strcpy(clue, message->clue);

			int length = strlen(clue);

			if (!isalpha(clue[length])) {
				clue[length + 1] = ' ';
				clue[length + 2] = '\0';
			}

			set_t *clues = teamp->clues;

			set_insert(clues, message->kragId, clue);

			// update GUI with new clue
			updateClues_I(clues);

			logMessage(filePath, messagep, "FROM", connection);
		}

	}

	else {
		printf("GSClue not valid\n");
	}
}

static void GSClaimedHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	if (GSClaimedValidate(message)) {

		// display the new krag
		krag_t *new = kragNew(message->latitude, message->longitude);

		set_insert(teamp->krags, message->kragId, new);
		updateMap_I(teamp->FAset, teamp->krags);

		updateKragsClaimed_I(++teamp->claimed);

		// check if the kragId was already inserted to the clues (to remove)
		if(set_find(teamp->clues, message->kragId) != NULL) {

			char *clue;

			if ((clue = set_find(teamp->clues, message->kragId)) != NULL) {
				clue[0] = '\0';
			}

			updateClues_I(teamp->clues);
		}
	}

	else {
		printf("GSClaimed not valid\n");
	}
}

static void GSSecretHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	if (GSSecretValidate(message)) {

		// copy message's secret to be the team's secret
		char *revealedString = calloc(141, 1);

		strcpy(revealedString, message->secret);

		// update GUI with newly revealed secret
		updateString_I(revealedString);

		logMessage(filePath, messagep, "FROM", connection);

		free(revealedString);
		revealedString = NULL;

	}

	else {
		printf("GSSecret not valid\n");
	}
}

static void GSResponseHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{
	// validate GS respondse, if valid log it and do nothing else
	if (GSResponseValidate(message)) {
		logMessage(filePath, messagep, "FROM", connection);
	}
}

static void teamRecordHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{

	// store each team's record, preparing to show their stats at GAME_OVER
	team_t *new = newTeam();

	// assign the necessary values from the message values
	new->claimed = message->numClaimed;
	new->numPlayers = message->numPlayers;

	hashtable_insert(teams, message->team, new);

	logMessage(filePath, messagep, "FROM", connection);
}

static void gameOverHandler(char *messagep, message_t *message, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams)
{

	gameOver_I(teams);

	logMessage(filePath, messagep, "FROM", connection);
}

/********** message validation functions ************/
// validate each specific message type so it can be handled accordingly
static bool gameStatusValidate(message_t *message, team_t *teamp) 
{
	// missing opCode
	if (message->opCode == NULL) {
		return false;
	}

	// gameId and guideId checks
	// never initialized
	if (message->guideId == NULL || message->gameId == NULL) {
		return false;
	} 

	// numClaimed or numKrags never initialized
	if (message->numClaimed == -600 || message->numKrags == -600) {
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
	// missing opCode
	if (message->opCode == NULL) {
		printf("no opCode\n");
		return false;
	}

	// gameId and pebbleId checks
	// never initialized when parsing
	if (message->gameId == NULL || message->pebbleId == NULL) {
		printf("gameId or pebbleId missing\n");
		return false;
	}

	// latitude, longitude, or lastContact were never assigned
	if (message->latitude == -600 || message->longitude == -600 || message->lastContact == -600) {
		return false;
	}

	// hexidecimal format check
	unsigned int gameIdFormat;
	if (sscanf(message->gameId, "%x", &gameIdFormat) != 1) {
		printf("not hexidecimal gameId\n");
		return false;
	}

	unsigned int pebbleIdFormat;
	if (sscanf(message->pebbleId, "%x", &pebbleIdFormat) != 1) {
		printf("not hexidecimal pebbleId\n");
		return false;
	}

	return true;
}

static bool GSClueValidate(message_t *message)
{
	// missing opCode
	if (message->opCode == NULL) {
		return false;
	}

	if (message->gameId == NULL || message->guideId == NULL || message->clue == NULL || message->kragId == NULL ) {
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

	unsigned int kragIdFormat;
	if (sscanf(message->kragId, "%x", &kragIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSClaimedValidate(message_t *message) 
{
	// missing opCode
	if (message->opCode == NULL) {
		return false;
	}

	// other missing strings
	if (message->gameId == NULL || message->guideId == NULL) {
		return false;
	}

	if (message->pebbleId == NULL || message->kragId == NULL) {
		return false;
	}

	// missing latitude or longitude
	if (message->latitude == -600 || message->longitude == -600) {
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

	unsigned int pebbleIdFormat;
	if (sscanf(message->pebbleId, "%x", &pebbleIdFormat) != 1) {
		return false;
	}

	unsigned int kragIdFormat;
	if (sscanf(message->kragId, "%x", &kragIdFormat) != 1) {
		return false;
	}

	return true;
}

static bool GSSecretValidate(message_t *message)
{
	// missing opCode
	if (message->opCode == NULL) {
		return false;
	}

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
	// missing opCode
	if (message->opCode == NULL) {
		return false;
	}

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
