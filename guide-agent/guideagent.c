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

// function declarations
int game(unsigned int guideId, char *team, char *player, char *host, char *port);


int main(int argc, char **argv) 
{
	// invalid argument count error
	if (argc != 6) {
		fprintf(stderr, "usage: guideagent guideId=... team=... player=... host=... port=...\n");
		exit(1);
	}

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
	if (strlen(team) > 10) {
		fprintf(stderr, "max team name length is 10 characters\n");
		exit(4);
	}

	char *player = malloc(strlen(playerTemp) - 7);
	player = strcpy(player, playerTemp + 7);
	// too long of a player name
	if (strlen(player) > 10) {
		fprintf(stderr, "max player name length is 10 characters\n");
		exit(4);
	}

	char *host = malloc(strlen(hostTemp) - 5);
	host = strcpy(host, hostTemp + 5);

	char *portTemp2 = malloc(strlen(portTemp) - 5);
	portTemp2 = strcpy(port, portTemp + 5);
	int port = atoi(portTemp2);
	// port given was not an integer
	if (port == 0) {
		fprintf(stderr, "port must be an integer\n");
		exit(4);
	}


	printf("%d\n", guideId);
	printf("%s\n", team);
	printf("%s\n", player);
	printf("%s\n", host);
	printf("%d\n", port);

	int exitStatus = game(guideId, team, player, host, port);

	// free the original parameters
	free(guideIdTemp2);
	free(team);
	free(player);
	free(host);
	free(port);

	exit(exitStatus);
}


int game(unsigned int guideId, char *team, char *player, char *host, char *port)
{


	return 0;
}