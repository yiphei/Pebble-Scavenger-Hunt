/*
 * message.c - common module to parse messages into arrays of strings
 *
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"

/******** function declaration ********/
static bool parseHelper(char *message, message_t *parsedMessage);

/******** function definition *********/

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
static bool parseHelper(char *message, message_t *parsedMessage)
{
	char *field;
	char *delim = "|";

	field = strtok(message, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		if (strncmp(field, "opCode=", 7) == 0) {
			if (parsedMessage->opCode != NULL) {
				fprintf(stderr, "duplicate opCode field given, message ignored\n");
				return false;
			}

			parsedMessage->opCode = malloc(strlen(field) - 7);
			strcpy(parsedMessage->opCode, field + 7);
		}

		if (strncmp(field, "respCode=", 9) == 0) {
			if (parsedMessage->respCode != NULL) {
				fprintf(stderr, "duplicate respCode field given, message ignored\n");
				return false;
			}

			parsedMessage->respCode = malloc(strlen(field) - 9);
			strcpy(parsedMessage->respCode, field + 9);
		}

		if (strncmp(field, "kragId=", 7) == 0) {
			if (parsedMessage->kragId != NULL) {
				fprintf(stderr, "duplicate kragId field given, message ignored\n");
				return false;
			}

			parsedMessage->kragId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->kragId, field + 7);
		}

		if (strncmp(field, "gameId=", 7) == 0) {
			if (parsedMessage->gameId != NULL) {
				fprintf(stderr, "duplicate gameId field given, message ignored\n");
				return false;
			}

			parsedMessage->gameId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->gameId, field + 7);
		}

		if (strncmp(field, "guideId=", 8) == 0) {
			if (parsedMessage->guideId != NULL) {
				fprintf(stderr, "duplicate guideId field given, message ignored\n");
				return false;
			}

			parsedMessage->guideId = malloc(strlen(field) - 8);
			strcpy(parsedMessage->guideId, field + 8);
		}

		if (strncmp(field, "pebbleId=", 9) == 0) {
			if (parsedMessage->pebbleId != NULL) {
				fprintf(stderr, "duplicate pebbleId field given, message ignored\n");
				return false;
			}

			parsedMessage->pebbleId = malloc(strlen(field) - 9);
			strcpy(parsedMessage->pebbleId, field + 9);
		}

		if (strncmp(field, "player=", 7) == 0) {
			if (parsedMessage->player != NULL) {
				fprintf(stderr, "duplicate player field given, message ignored\n");
				return false;
			}

			parsedMessage->player = malloc(strlen(field) - 7);
			strcpy(parsedMessage->player, field + 7);
		}

		if (strncmp(field, "team=", 5) == 0) {
			if (parsedMessage->team != NULL) {
				fprintf(stderr, "duplicate team field given, message ignored\n");
				return false;
			}

			parsedMessage->team = malloc(strlen(field) - 5);
			strcpy(parsedMessage->team, field + 5);
		}

		if (strncmp(field, "hint=", 5) == 0) {
			if (parsedMessage->hint != NULL) {
				fprintf(stderr, "duplicate hint field given, message ignored\n");
				return false;
			}

			parsedMessage->hint = malloc(strlen(field) - 5);
			strcpy(parsedMessage->hint, field + 5);
		}

		if (strncmp(field, "clue=", 5) == 0) {
			if (parsedMessage->clue != NULL) {
				fprintf(stderr, "duplicate clue field given, message ignored\n");
				return false;
			}

			parsedMessage->clue = malloc(strlen(field) - 5);
			strcpy(parsedMessage->clue, field + 5);
		}

		if (strncmp(field, "secret=", 7) == 0) {
			if (parsedMessage->secret != NULL) {
				fprintf(stderr, "duplicate secret field given, message ignored\n");
				return false;
			}

			parsedMessage->secret = malloc(strlen(field) - 7);
			strcpy(parsedMessage->secret, field + 7);
		}

		field = strtok(NULL, delim);
	}

	return true;
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