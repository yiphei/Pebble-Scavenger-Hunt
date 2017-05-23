/*
 * parser.c - common module to parse messages into arrays of strings
 *
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/******** function declaration ********/
message_t *newMessage(char *message);
void parseMessage(char *message, message_t *parsedMessage);

// struct allows storage of all message field types
typedef struct message 
{
	char *opCode;
	char *respCode;
	char *kragId;
	char *gameId;
	char *guideId;
	char *pebbleId;
	int lastContact;
	long latitude;
	long longitude;
	int numPlayers;
	int numClaimed;
	int numKrags;
	int statusReq;
	char *player;
	char *team;
	char *hint;
	char *clue;
	char *secret;
} message_t;

message_t * newMessage(char *message)
{
	message_t *parsedMessage = malloc(sizeof(message_t));

	parseMessage(char *message, message_t *parsedMessage);

	return parsedMessage;
}

void parseMessage(char *message, message_t *parsedMessage)
{
	
}