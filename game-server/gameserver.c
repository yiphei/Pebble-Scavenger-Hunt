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

/******* functions *******/
int server(int gameId, char* kff, char* sf, int port);

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
	int result = server(gameId, kff, sf, port);

	exit(result); // exit with the correct exit code
}


/*
* Game Server function that creates and runs the server
* Takes a game ID, krag file path, secret file path, and port number
*
*/
int server(int gameId, char* kff, char* sf, int port)
{
	// initialize the server
	if(!startServer()){
		fprintf(stderr, "Unable to start the server\n");
		return 5;
	}

	return 0; // return 0 for success
}

