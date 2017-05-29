/*
 * message.c - common module to parse messages into arrays of strings
 *
 *
 * GREP, CS50, May 2017
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"

/******** function declaration ********/
int parseHelper(char *message, message_t *parsedMessage);
/******** function definition *********/

// parses message into a message struct using helper function to factor
message_t * parseMessage(char *message)
{
	message_t *parsedMessage = malloc(sizeof(message_t));

	int errorCode;

	errorCode = parseHelper(message, parsedMessage);

	parsedMessage->errorCode = errorCode;

	return parsedMessage;
}

/* 
compares each field in the message to known formats "type=..." and 
assigns the variable of the given message_t * to the value on the right hand
side of the variable assignment in the message; does not return because it just
modifies the pointer parseMessage passes as a parameter 

returns an int signifying to the Game Server if there are certain errors in
the message syntax. 
0: success
1: duplicate field
2: invalid fieldName
3: invalid message (types do not match)
 */
int parseHelper(char *message, message_t *parsedMessage)
{
	char *field;
	char *delim = "|";

	// arbitrary value assignments to check for duplicate fields later
	parsedMessage->lastContact = -600;
	parsedMessage->latitude = -600;
	parsedMessage->longitude = -600;
	parsedMessage->numPlayers = -600;
	parsedMessage->numClaimed = -600;
	parsedMessage->numKrags = -600;
	parsedMessage->statusReq = -600;

	field = strtok(message, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		if (strncmp(field, "opCode=", 7) == 0) {
			if (parsedMessage->opCode != NULL) {
				return 1;
			}

			parsedMessage->opCode = malloc(strlen(field) - 7);
			strcpy(parsedMessage->opCode, field + 7);
		}

		else if (strncmp(field, "respCode=", 9) == 0) {
			if (parsedMessage->respCode != NULL) {
				return 1;
			}

			parsedMessage->respCode = malloc(strlen(field) - 9);
			strcpy(parsedMessage->respCode, field + 9);
		}

		else if (strncmp(field, "kragId=", 7) == 0) {
			if (parsedMessage->kragId != NULL) {
				return 1;
			}

			parsedMessage->kragId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->kragId, field + 7);
		}

		else if (strncmp(field, "gameId=", 7) == 0) {
			if (parsedMessage->gameId != NULL) {
				return 1;
			}

			parsedMessage->gameId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->gameId, field + 7);
		}

		else if (strncmp(field, "guideId=", 8) == 0) {
			if (parsedMessage->guideId != NULL) {
				return 1;
			}

			parsedMessage->guideId = malloc(strlen(field) - 8);
			strcpy(parsedMessage->guideId, field + 8);
		}

		else if (strncmp(field, "pebbleId=", 9) == 0) {
			if (parsedMessage->pebbleId != NULL) {
				return 1;
			}

			parsedMessage->pebbleId = malloc(strlen(field) - 9);
			strcpy(parsedMessage->pebbleId, field + 9);
		}

		else if (strncmp(field, "player=", 7) == 0) {
			if (parsedMessage->player != NULL) {
				return 1;
			}

			parsedMessage->player = malloc(strlen(field) - 7);
			strcpy(parsedMessage->player, field + 7);
		}

		else if (strncmp(field, "team=", 5) == 0) {
			if (parsedMessage->team != NULL) {
				return 1;
			}

			parsedMessage->team = malloc(strlen(field) - 5);
			strcpy(parsedMessage->team, field + 5);
		}

		else if (strncmp(field, "hint=", 5) == 0) {
			if (parsedMessage->hint != NULL) {
				return 1;
			}

			parsedMessage->hint = malloc(strlen(field) - 5);
			strcpy(parsedMessage->hint, field + 5);
		}

		else if (strncmp(field, "clue=", 5) == 0) {
			if (parsedMessage->clue != NULL) {
				return 1;
			}

			parsedMessage->clue = malloc(strlen(field) - 5);
			strcpy(parsedMessage->clue, field + 5);
		}

		else if (strncmp(field, "secret=", 7) == 0) {
			if (parsedMessage->secret != NULL) {
				return 1;
			}

			parsedMessage->secret = malloc(strlen(field) - 7);
			strcpy(parsedMessage->secret, field + 7);
		}

		else if (strncmp(field, "text=", 5) == 0) {
			if (parsedMessage->text != NULL) {
				return 1;
			}

			parsedMessage->text = malloc(strlen(field) - 5);
			strcpy(parsedMessage->text, field + 5);
		}

		else if (strncmp(field, "lastContact=", 12) == 0) {
			if (parsedMessage->lastContact != -600) {
				return 1;
			}

			char *lastContactTemp = malloc(strlen(field) - 12);
			strcpy(lastContactTemp, field + 12);

			int lastContact;

			if (sscanf(lastContactTemp, "%d", &lastContact) != 1) {
				return 3;
			}

			parsedMessage->lastContact = lastContact;

			free(lastContactTemp);
			lastContactTemp = NULL;
		}

		else if (strncmp(field, "latitude=", 9) == 0) {
			if (parsedMessage->latitude != -600) {
				return 1;
			}

			char *latitudeTemp = malloc(strlen(field) - 9);
			strcpy(latitudeTemp, field + 9);

			double latitude;

			if (sscanf(latitudeTemp, "%lf", &latitude) != 1) {
				return 3;
			}

			parsedMessage->latitude = latitude;

			free(latitudeTemp);
			latitudeTemp = NULL;
		}

		else if (strncmp(field, "longitude=", 10) == 0) {
			if (parsedMessage->longitude != -600) {
				return 1;
			}

			char *longitudeTemp = malloc(strlen(field) - 10);
			strcpy(longitudeTemp, field + 10);

			double longitude;

			if (sscanf(longitudeTemp, "%lf", &longitude) != 1) {
				return 3;
			}

			parsedMessage->longitude = longitude;

			free(longitudeTemp);
			longitudeTemp = NULL;
		}

		else if (strncmp(field, "numPlayers=", 11) == 0) {
			if (parsedMessage->numPlayers != -600) {
				return 1;
			}

			char *numPlayersTemp = malloc(strlen(field) - 11);
			strcpy(numPlayersTemp, field + 11);

			int numPlayers;
			if (sscanf(numPlayersTemp, "%d", &numPlayers) != 1) {
				return 3;
			}
			parsedMessage->numPlayers = numPlayers;

			free(numPlayersTemp);
			numPlayersTemp = NULL;
		}

		else if (strncmp(field, "numClaimed=", 11) == 0) {
			if (parsedMessage->numClaimed != -600) {
				return 1;
			}

			char *numClaimedTemp = malloc(strlen(field) - 11);
			strcpy(numClaimedTemp, field + 11);

			int numClaimed;

			if (sscanf(numClaimedTemp, "%d", &numClaimed) != 1) {
				return 3;
			}

			parsedMessage->numClaimed = numClaimed;

			free(numClaimedTemp);
			numClaimedTemp = NULL;
		}

		else if (strncmp(field, "numKrags=", 9) == 0) {
			if (parsedMessage->numKrags != -600) {
				return 1;
			}

			char *numKragsTemp = malloc(strlen(field) - 9);
			strcpy(numKragsTemp, field + 9);

			int numKrags;

			if(sscanf(numKragsTemp, "%d", &numKrags) != 1) {
				return 3;

			}

			parsedMessage->numKrags = numKrags;

			free(numKragsTemp);
			numKragsTemp = NULL;
		}

		else if (strncmp(field, "statusReq=", 10) == 0) {
			if (parsedMessage->statusReq != -600) {
				return 1;
			}

			char *statusReqTemp = malloc(strlen(field) - 10);
			strcpy(statusReqTemp, field + 10);

			int statusReq;

			if (sscanf(statusReqTemp, "%d", &statusReq) != 1) {
				return 3;
			}

			parsedMessage->statusReq = statusReq;

			free(statusReqTemp);
			statusReqTemp = NULL;
		}

		else {
			return 2;
		}

		field = strtok(NULL, delim);
	}

	return 0;
}

void deleteMessage(message_t *message) {
	if (message->opCode != NULL){
		free(message->opCode);
		message->opCode = NULL;
	}

	if (message->respCode != NULL) {
		free(message->respCode);
		message->respCode = NULL;
	}

	if (message->kragId != NULL) {
		free(message->kragId);
		message->kragId = NULL;
	}

	if (message->gameId != NULL) {
		free(message->gameId);
		message->gameId = NULL;
	}

	if (message->guideId != NULL) { 
		free(message->guideId);
		message->guideId = NULL;
	}

	if (message->pebbleId != NULL) {
		free(message->pebbleId);
		message->pebbleId = NULL;
	}

	if (message->player != NULL) {
		free(message->player);
		message->player = NULL;
	}

	if (message->team != NULL) {
		free(message->team);
		message->team = NULL;
	}

	if (message->hint != NULL) {
		free(message->hint);
		message->hint = NULL;
	}

	if (message->clue != NULL) {
		free(message->clue);
		message->clue = NULL;
	}

	if (message->secret != NULL) {
		free(message->secret);
		message->clue = NULL;
	}

	if (message->text != NULL) {
		free(message->secret);
		message->secret = NULL;
	}

	free(message);
	message = NULL;

}

