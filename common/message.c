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
static int parseHelper(char *message, message_t *parsedMessage);
static char * myStrtok(char *s, char *delim);
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
static int parseHelper(char *message, message_t *parsedMessage)
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

	field = myStrtok(message, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		if (strncmp(field, "opCode=", 7) == 0) {
			if (parsedMessage->opCode != NULL) {
				fprintf(stderr, "duplicate opCode field given, message ignored\n");
				return 1;
			}

			parsedMessage->opCode = malloc(strlen(field) - 7);
			strcpy(parsedMessage->opCode, field + 7);
		}

		else if (strncmp(field, "respCode=", 9) == 0) {
			if (parsedMessage->respCode != NULL) {
				fprintf(stderr, "duplicate respCode field given, message ignored\n");
				return 1;
			}

			parsedMessage->respCode = malloc(strlen(field) - 9);
			strcpy(parsedMessage->respCode, field + 9);
		}

		else if (strncmp(field, "kragId=", 7) == 0) {
			if (parsedMessage->kragId != NULL) {
				fprintf(stderr, "duplicate kragId field given, message ignored\n");
				return 1;
			}

			parsedMessage->kragId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->kragId, field + 7);
		}

		else if (strncmp(field, "gameId=", 7) == 0) {
			if (parsedMessage->gameId != NULL) {
				fprintf(stderr, "duplicate gameId field given, message ignored\n");
				return 1;
			}

			parsedMessage->gameId = malloc(strlen(field) - 7);
			strcpy(parsedMessage->gameId, field + 7);
		}

		else if (strncmp(field, "guideId=", 8) == 0) {
			if (parsedMessage->guideId != NULL) {
				fprintf(stderr, "duplicate guideId field given, message ignored\n");
				return 1;
			}

			parsedMessage->guideId = malloc(strlen(field) - 8);
			strcpy(parsedMessage->guideId, field + 8);
		}

		else if (strncmp(field, "pebbleId=", 9) == 0) {
			if (parsedMessage->pebbleId != NULL) {
				fprintf(stderr, "duplicate pebbleId field given, message ignored\n");
				return 1;
			}

			parsedMessage->pebbleId = malloc(strlen(field) - 9);
			strcpy(parsedMessage->pebbleId, field + 9);
		}

		else if (strncmp(field, "player=", 7) == 0) {
			if (parsedMessage->player != NULL) {
				fprintf(stderr, "duplicate player field given, message ignored\n");
				return 1;
			}

			parsedMessage->player = malloc(strlen(field) - 7);
			strcpy(parsedMessage->player, field + 7);
		}

		else if (strncmp(field, "team=", 5) == 0) {
			if (parsedMessage->team != NULL) {
				fprintf(stderr, "duplicate team field given, message ignored\n");
				return 1;
			}

			parsedMessage->team = malloc(strlen(field) - 5);
			strcpy(parsedMessage->team, field + 5);
		}

		else if (strncmp(field, "hint=", 5) == 0) {
			if (parsedMessage->hint != NULL) {
				fprintf(stderr, "duplicate hint field given, message ignored\n");
				return 1;
			}

			parsedMessage->hint = malloc(strlen(field) - 5);
			strcpy(parsedMessage->hint, field + 5);
		}

		else if (strncmp(field, "clue=", 5) == 0) {
			if (parsedMessage->clue != NULL) {
				fprintf(stderr, "duplicate clue field given, message ignored\n");
				return 1;
			}

			parsedMessage->clue = malloc(strlen(field) - 5);
			strcpy(parsedMessage->clue, field + 5);
		}

		else if (strncmp(field, "secret=", 7) == 0) {
			if (parsedMessage->secret != NULL) {
				fprintf(stderr, "duplicate secret field given, message ignored\n");
				return 1;
			}

			parsedMessage->secret = malloc(strlen(field) - 7);
			strcpy(parsedMessage->secret, field + 7);
		}

		else if (strncmp(field, "text=", 5) == 0) {
			if (parsedMessage->text != NULL) {
				fprintf(stderr, "duplicate team field given, message ignored\n");
				return 1;
			}

			parsedMessage->text = malloc(strlen(field) - 5);
			strcpy(parsedMessage->text, field + 5);
		}

		else if (strncmp(field, "lastContact=", 12) == 0) {
			if (parsedMessage->lastContact != -600) {
				fprintf(stderr, "duplicate lastContact field given, message ignored\n");
				return 1;
			}

			char *lastContactTemp = malloc(strlen(field) - 12);
			strcpy(lastContactTemp, field + 12);

			int lastContact;

			if (sscanf(lastContactTemp, "%d", &lastContact) != 1) {
				fprintf(stderr, "invalid message, lastContact must be an int\n");
				return 3;
			}

			parsedMessage->lastContact = lastContact;

			free(lastContactTemp);
		}

		else if (strncmp(field, "latitiude=", 9) == 0) {
			if (parsedMessage->latitude != -600) {
				fprintf(stderr, "duplicate latitude field given, message ignored\n");
				return 1;
			}

			char *latitudeTemp = malloc(strlen(field) - 9);
			strcpy(latitudeTemp, field + 9);

			double latitude;

			if (sscanf(latitudeTemp, "%lf", &latitude) != 1) {
				fprintf(stderr, "invalid message, latitude must be a double\n");
				return 3;
			}

			parsedMessage->latitude = latitude;

			free(latitudeTemp);
		}

		else if (strncmp(field, "longitude=", 10) == 0) {
			if (parsedMessage->longitude != -600) {
				fprintf(stderr, "duplicate longitude field given, message ignored\n");
				return 1;
			}

			char *longitudeTemp = malloc(strlen(field) - 10);
			strcpy(longitudeTemp, field + 10);

			double longitude;

			if (sscanf(longitudeTemp, "%lf", &longitude) != 1) {
				fprintf(stderr, "invalid message, longitude must be a double\n");
				return 3;
			}

			parsedMessage->longitude = longitude;

			free(longitudeTemp);
		}

		else if (strncmp(field, "numPlayers=", 11) == 0) {
			if (parsedMessage->numPlayers != -600) {
				fprintf(stderr, "duplicate numPlayers field given, message ignored\n");
				return 1;
			}

			char *numPlayersTemp = malloc(strlen(field) - 11);
			strcpy(numPlayersTemp, field + 11);

			int numPlayers;
			if (sscanf(numPlayersTemp, "%d", &numPlayers) != 1) {
				fprintf(stderr, "invalid message, numPlayers must be an int\n");
				return 3;
			}
			parsedMessage->numPlayers = numPlayers;

			free(numPlayersTemp);
		}

		else if (strncmp(field, "numClaimed=", 11) == 0) {
			if (parsedMessage->numClaimed != -600) {
				fprintf(stderr, "duplicate numClaimed field given, message ignored\n");
				return 1;
			}

			char *numClaimedTemp = malloc(strlen(field) - 11);
			strcpy(numClaimedTemp, field + 11);

			int numClaimed;

			if (sscanf(numClaimedTemp, "%d", &numClaimed) != 1) {
				fprintf(stderr, "invalid message, numClaimed must be an int\n");
				return 3;
			}

			parsedMessage->numClaimed = numClaimed;

			free(numClaimedTemp);
		}

		else if (strncmp(field, "numKrags=", 9) == 0) {
			if (parsedMessage->numKrags != -600) {
				fprintf(stderr, "duplicate numKrags field given, message ignored\n");
				return 1;
			}

			char *numKragsTemp = malloc(strlen(field) - 9);
			strcpy(numKragsTemp, field + 9);

			int numKrags;

			if(sscanf(numKragsTemp, "%d", &numKrags) != 1) {
				fprintf(stderr, "invalid message, numKrags must be an int\n");
				return 3;

			}

			parsedMessage->numKrags = numKrags;

			free(numKragsTemp);
		}

		else if (strncmp(field, "statusReqs=", 10) == 0) {
			if (parsedMessage->statusReq != -600) {
				fprintf(stderr, "duplicate statusReq field given, message ignored\n");
				return 1;
			}

			char *statusReqTemp = malloc(strlen(field) - 10);
			strcpy(statusReqTemp, field + 10);

			int statusReq;

			if (sscanf(statusReqTemp, "%d", &statusReq) != 1) {
				fprintf(stderr, "invalid message, statusReq must be an int\n");
				return 3;
			}

			parsedMessage->statusReq = statusReq;

			free(statusReqTemp);
		}

		else {
			fprintf(stderr, "invalid fieldName\n");
			return 2;
		}

		field = myStrtok(NULL, delim);
	}

	return 0;
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

	if (message->text != NULL) {
		free(message->secret);
	}

	free(message);

}

// This strtok works with pebble:
/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

char *
myStrtok(char *s, char *delim)
{
	char *spanp;
	int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}


