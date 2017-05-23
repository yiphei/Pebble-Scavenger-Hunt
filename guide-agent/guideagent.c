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
#include <ncurses.h>
#include "display.c"

/******** function declarations ********/
int game(unsigned int guideId, char *team, char *player, char *host, int port);
static bool openSocket(char *host, int port);

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
	char *guideIdTemp2 = malloc(strlen(guideIdTemp) - 8);
	guideIdTemp2 = strcpy(guideIdTemp2, guideIdTemp + 8);

	// invalid guideId length
	if (strlen(guideIdTemp2) > 8 || strlen(guideIdTemp2) == 0) {
		fprintf(stderr, "guideId should be 1-8 characters\n");
	}

	// invalid hexidecimal format
	unsigned int guideId;
	if (sscanf(guideIdTemp2, "%x", &guideId) != 1) {
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

	int exitStatus = game(guideId, team, player, host, port);

	// free the original parameters
	free(guideIdTemp2);
	free(team);
	free(player);
	free(host);
	free(portTemp2);

	exit(exitStatus);
}


int game(unsigned int guideId, char *team, char *player, char *host, int port)
{
	if (!openSocket(host, port)) {

	}

	return 0;
}

// helper function for game to connect to the Game Server
static bool openSocket(char *host, int port)
{
	// connect to host by supplied host name
	struct hostent *hostp = gethostbyname(host);

	if (hostp == NULL) {
		fprintf(stderr, "unknown host\n");
		return false;
	}

	// initialize fields of the Game Server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port); // bind to server specific IP

	int socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (socket < 0) {
		fprintf(stderr, "error opening socket\n");
		return false;
	}

	if (bind(socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
		fprintf(stderr, "error binding Guide Agent socket to server\n");
		return false;
	}

	return true;
}