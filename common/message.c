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

message_t *newMessage() 
{
	message_t *parsedMessage = calloc(sizeof(message_t), 1);

	if (parsedMessage == NULL) {
		return NULL;
	}

	parsedMessage->opCode = NULL;
	parsedMessage->respCode = NULL;
	parsedMessage->kragId = NULL;
	parsedMessage->gameId = NULL;
	parsedMessage->guideId = NULL;
	parsedMessage->pebbleId = NULL;
	parsedMessage->player = NULL;
	parsedMessage->team = NULL;
	parsedMessage->hint = NULL;
	parsedMessage->clue = NULL;
	parsedMessage->secret = NULL;
	parsedMessage->text = NULL;

	// arbitrary value assignments to check for duplicate fields later
	parsedMessage->lastContact = -600;
	parsedMessage->latitude = -600;
	parsedMessage->longitude = -600;
	parsedMessage->numPlayers = -600;
	parsedMessage->numClaimed = -600;
	parsedMessage->numKrags = -600;
	parsedMessage->statusReq = -600;

	return parsedMessage;
}

// parses message into a message struct using helper function to factor
message_t * parseMessage(char *message)
{
	message_t *parsedMessage = newMessage();

	if (parsedMessage == NULL) {
		return NULL;
	}

	int errorCode = parseHelper(message, parsedMessage);

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
4: given field is not the correct length
5: invalid latitude/longitude (too big or too small)
 */
int parseHelper(char *message, message_t *parsedMessage)
{
	char *field;
	char *delim = "|";

	char *messageModifier = calloc(strlen(message) + 1, 1);
	strcpy(messageModifier, message);
	strcat(messageModifier, "\0");

	field = strtok(messageModifier, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		if (strncmp(field, "opCode=", 7) == 0) {
			if (parsedMessage->opCode != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->opCode = calloc(strlen(field) - 6, 1);
			strcpy(parsedMessage->opCode, field + 7);
			strcat(parsedMessage->opCode, "\0");
		}

		else if (strncmp(field, "respCode=", 9) == 0) {
			if (parsedMessage->respCode != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->respCode = calloc(strlen(field) - 8, 1);
			strcpy(parsedMessage->respCode, field + 9);
			strcat(parsedMessage->respCode, "\0");
		}

		else if (strncmp(field, "kragId=", 7) == 0) {
			if (parsedMessage->kragId != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->kragId = calloc(strlen(field) - 6, 1);
			strcpy(parsedMessage->kragId, field + 7);
			strcat(parsedMessage->kragId, "\0");

			if (strlen(parsedMessage->kragId) > 4) {
				return 4;
			}
		}

		else if (strncmp(field, "gameId=", 7) == 0) {
			if (parsedMessage->gameId != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->gameId = calloc(strlen(field) - 6, 1);
			strcpy(parsedMessage->gameId, field + 7);
			strcat(parsedMessage->gameId, "\0");

			if (strlen(parsedMessage->gameId) > 8) {
				return 4;
			}
		}

		else if (strncmp(field, "guideId=", 8) == 0) {
			if (parsedMessage->guideId != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->guideId = calloc(strlen(field) - 7, 1);
			strcpy(parsedMessage->guideId, field + 8);
			strcat(parsedMessage->guideId, "\0");

			if (strlen(parsedMessage->guideId) > 8) {
				return 4;
			}
		}

		else if (strncmp(field, "pebbleId=", 9) == 0) {
			if (parsedMessage->pebbleId != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->pebbleId = calloc(strlen(field) - 8, 1);
			strcpy(parsedMessage->pebbleId, field + 9);
			strcat(parsedMessage->pebbleId, "\0");

			if (strlen(parsedMessage->pebbleId) != 8 && strcmp(parsedMessage->pebbleId, "*") != 0) {
				return 4;
			}
		}

		else if (strncmp(field, "player=", 7) == 0) {
			if (parsedMessage->player != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->player = calloc(strlen(field) - 6, 1);
			NormalizeWord(field);
			strcpy(parsedMessage->player, field + 7);
			strcat(parsedMessage->player, "\0");

			if (strlen(parsedMessage->player) > 10) {
				return 4;
			}
		}

		else if (strncmp(field, "team=", 5) == 0) {
			if (parsedMessage->team != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->team = calloc(strlen(field) - 4, 1);
			NormalizeWord(field);
			strcpy(parsedMessage->team, field + 5);
			strcat(parsedMessage->team, "\0");

			if (strlen(parsedMessage->team) > 10) {
				return 4;
			}
		}

		else if (strncmp(field, "hint=", 5) == 0) {
			if (parsedMessage->hint != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->hint = calloc(strlen(field) - 4, 1);
			strcpy(parsedMessage->hint, field + 5);
			strcat(parsedMessage->hint, "\0");

			if (strlen(parsedMessage->hint) > 140) {
				return 4;
			}
		}

		else if (strncmp(field, "clue=", 5) == 0) {
			if (parsedMessage->clue != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->clue = calloc(strlen(field) - 4, 1);
			strcpy(parsedMessage->clue, field + 5);
			strcat(parsedMessage->clue, "\0");

			if (strlen(parsedMessage->clue) > 140) {
				return 4;
			}
		}

		else if (strncmp(field, "secret=", 7) == 0) {
			if (parsedMessage->secret != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->secret = calloc(strlen(field) - 7, 1);
			strcpy(parsedMessage->secret, field + 7);

			if (strlen(parsedMessage->secret) > 140) {
				return 4;
			}
		}

		else if (strncmp(field, "text=", 5) == 0) {
			if (parsedMessage->text != NULL) {
				free(messageModifier);
				return 1;
			}

			parsedMessage->text = calloc(strlen(field) - 4, 1);
			strcpy(parsedMessage->text, field + 5);
			strcat(parsedMessage->text, "\0");

			if (strlen(parsedMessage->text) > 140) {
				return 4;
			}
		}

		else if (strncmp(field, "lastContact=", 12) == 0) {
			if (parsedMessage->lastContact != -600) {
				free(messageModifier);
				return 1;
			}

			char *lastContactTemp = calloc(strlen(field) - 12, 1);
			strcpy(lastContactTemp, field + 12);

			int lastContact;

			if (sscanf(lastContactTemp, "%d", &lastContact) != 1) {
				free(messageModifier);
				free(lastContactTemp);
				return 3;
			}

			parsedMessage->lastContact = lastContact;

			free(lastContactTemp);
			lastContactTemp = NULL;
		}

		else if (strncmp(field, "latitude=", 9) == 0) {
			if (parsedMessage->latitude != -600) {
				free(messageModifier);
				return 1;
			}

			char *latitudeTemp = calloc(strlen(field) - 9, 1);
			strcpy(latitudeTemp, field + 9);

			double latitude;

			if (sscanf(latitudeTemp, "%lf", &latitude) != 1) {
				free(messageModifier);
				free(latitudeTemp);
				return 3;
			}

			parsedMessage->latitude = latitude;

			free(latitudeTemp);
			latitudeTemp = NULL;

			if (parsedMessage->latitude > 90.0 || parsedMessage->latitude < -90.0) {
				return 5;
			}
		}

		else if (strncmp(field, "longitude=", 10) == 0) {
			if (parsedMessage->longitude != -600) {
				free(messageModifier);
				return 1;
			}

			char *longitudeTemp = calloc(strlen(field) - 10, 1);
			strcpy(longitudeTemp, field + 10);

			double longitude;

			if (sscanf(longitudeTemp, "%lf", &longitude) != 1) {
				free(messageModifier);
				free(longitudeTemp);
				return 3;
			}

			parsedMessage->longitude = longitude;

			free(longitudeTemp);
			longitudeTemp = NULL;

			if (parsedMessage->longitude > 180.0 || parsedMessage->longitude < -180.0) {
				return 5;
			}
		}

		else if (strncmp(field, "numPlayers=", 11) == 0) {
			if (parsedMessage->numPlayers != -600) {
				free(messageModifier);
				return 1;
			}

			char *numPlayersTemp = calloc(strlen(field) - 11, 1);
			strcpy(numPlayersTemp, field + 11);

			int numPlayers;
			if (sscanf(numPlayersTemp, "%d", &numPlayers) != 1) {
				free(messageModifier);
				free(numPlayersTemp);
				return 3;
			}
			parsedMessage->numPlayers = numPlayers;

			free(numPlayersTemp);
			numPlayersTemp = NULL;
		}

		else if (strncmp(field, "numClaimed=", 11) == 0) {
			if (parsedMessage->numClaimed != -600) {
				free(messageModifier);
				return 1;
			}

			char *numClaimedTemp = calloc(strlen(field) - 11, 1);
			strcpy(numClaimedTemp, field + 11);

			int numClaimed;

			if (sscanf(numClaimedTemp, "%d", &numClaimed) != 1) {
				free(messageModifier);
				free(numClaimedTemp);
				return 3;
			}

			parsedMessage->numClaimed = numClaimed;

			free(numClaimedTemp);
			numClaimedTemp = NULL;
		}

		else if (strncmp(field, "numKrags=", 9) == 0) {
			if (parsedMessage->numKrags != -600) {
				free(messageModifier);
				return 1;
			}

			char *numKragsTemp = calloc(strlen(field) - 9, 1);
			strcpy(numKragsTemp, field + 9);

			int numKrags;

			if(sscanf(numKragsTemp, "%d", &numKrags) != 1) {
				free(messageModifier);
				free(numKragsTemp);
				return 3;

			}

			parsedMessage->numKrags = numKrags;

			free(numKragsTemp);
			numKragsTemp = NULL;
		}

		else if (strncmp(field, "statusReq=", 10) == 0) {
			if (parsedMessage->statusReq != -600) {
				free(messageModifier);
				return 1;
			}

			char *statusReqTemp = calloc(strlen(field) - 10, 1);
			strcpy(statusReqTemp, field + 10);

			int statusReq;

			if (sscanf(statusReqTemp, "%d", &statusReq) != 1) {
				free(messageModifier);
				free(statusReqTemp);
				return 3;
			}

			parsedMessage->statusReq = statusReq;

			free(statusReqTemp);
			statusReqTemp = NULL;
		}

		else {
			free(messageModifier);
			return 2;
		}

		field = strtok(NULL, delim);
	}

	free(messageModifier);

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

