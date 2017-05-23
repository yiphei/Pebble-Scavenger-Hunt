/*
 * message.c - common module to parse messages into arrays of strings
 *
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"

/******** function declaration ********/
static void parseHelper(char *message, message_t *parsedMessage);

/******** function definitino *********/

// parses message into a message struct using helper function to factor
message_t * parseMessage(char *message)
{
	message_t *parsedMessage = malloc(sizeof(message_t));

	parseHelper(message, parsedMessage);

	return parsedMessage;
}

/* compares each field in the message to known formats "type=..." and 
assigns the variable of the given message_t * to the value on the right hand
side of the variable assignment in the message; does not return because it just
modifies the pointer parseMessage passes as a parameter */
static void parseHelper(char *message, message_t *parsedMessage)
{
	char *field;
	char *delim = "|";

	field = strtok(message, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		if (strncmp(field, "opCode=", 7) == 0) {
			parsedMessage->opCode = malloc(strlen(field) - 7);
			strcpy(parsedMessage->opCode, field + 7);
		}

		if (strncmp(field, "respCode=", 9) == 0) {
			parsedMessage->respCode = malloc(strlen(field) - 9);
			strcpy(parsedMessage->respCode, field + 9);
		}

		if (strncmp(field, "kragId=", 7) == 0) {
			parsedMessage->kragId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->kragId, field + 7);
		}

		if (strncmp(field, "gameId=", 7) == 0) {
			parsedMessage->gameId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->gameId, field + 7);
		}

		if (strncmp(field, "guideId=", 8) == 0) {
			parsedMessage->guideId = malloc(strlen(field) - 8);
			strcpy(parsedMessage->guideId, field + 8);
		}

		if (strncmp(field, "pebbleId=", 9) == 0) {
			parsedMessage->pebbleId = malloc(strlen(field) - 9);
			strcpy(parsedMessage->pebbleId, field + 9);
		}

		if (strncmp(field, "player=", 7) == 0) {
			parsedMessage->player = malloc(strlen(field) - 7);
			strcpy(parsedMessage->player, field + 7);
		}

		if (strncmp(field, "team=", 5) == 0) {
			parsedMessage->team = malloc(strlen(field) - 5);
			strcpy(parsedMessage->team, field + 5);
		}

		if (strncmp(field, "hint=", 5) == 0) {
			parsedMessage->hint = malloc(strlen(field) - 5);
			strcpy(parsedMessage->hint, field + 5);
		}

		if (strncmp(field, "clue=", 5) == 0) {
			parsedMessage->clue = malloc(strlen(field) - 5);
			strcpy(parsedMessage->clue, field + 5);
		}

		if (strncmp(field, "secret=", 7) == 0) {
			parsedMessage->secret = malloc(strlen(field) - 7);
			strcpy(parsedMessage->secret, field + 7);
		}

		field = strtok(NULL, delim);
	}
}

void deleteMessage(message_t *message) {
	if (message->opCode != NULL){
		free(message->opCode);
	}

	if (message->respCode != NULL) {
		free(message->respCode);
	}

	if (message->kragId != NULL) {
		free(message->kragId);
	}

	if (message->gameId != NULL) {
		free(message->gameId);
	}

	if (message->guideId != NULL) { 
		free(message->guideId);
	}

	if (message->pebbleId != NULL) {
		free(message->pebbleId);
	}

	if (message->player != NULL) {
		free(message->player);
	}

	if (message->team != NULL) {
		free(message->team);
	}

	if (message->hint != NULL) {
		free(message->hint);
	}

	if (message->clue != NULL) {
		free(message->clue);
	}

	if (message->secret != NULL) {
		free(message->secret);
	}

	free(message);
}