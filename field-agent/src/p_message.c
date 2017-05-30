/*
 * p_message.c - see p_message.h for more info
 *
 *
 * Paolo Takagi-Atilano, May 2017
 */

#include <stddef.h>
#include <stdlib.h>
#include <pebble.h>
#include <string.h>
#include <stdbool.h>
#include "p_message.h"

/******** function declaration ********/
static int parse_helper(char *message, p_message_t *parsed_message);
char * my_strtok(char *s, char *delim);
static p_message_t *new_message();
/******** function definition *********/

// parses message into a message struct using helper function to factor
p_message_t * parse_message(char *message)
{
	p_message_t *tempmsg = new_message();

	char *temp = malloc(strlen(message)+1);
	strcpy(temp, message);
	if (strncmp(temp, "ACK", 3) == 0 || strncmp(temp, "NACK", 4) == 0) {
		free(temp);
		return NULL;
	}
	free(temp);
	//p_message_t *parsed_message = malloc(sizeof(p_message_t));

	int error_code;

	APP_LOG(APP_LOG_LEVEL_INFO,"msg--: %s", message);

	error_code = parse_helper(message, tempmsg);

	tempmsg->error_code = error_code;

	return tempmsg;
}

static p_message_t *new_message() 
{
p_message_t *parsedMessage = calloc(sizeof(p_message_t), 1);

if (parsedMessage == NULL) {
return NULL;
}

parsedMessage->op_code = NULL;
parsedMessage->resp_code = NULL;
parsedMessage->krag_id = NULL;
parsedMessage->game_id = NULL;
parsedMessage->guide_id = NULL;
parsedMessage->pebble_id = NULL;
parsedMessage->player = NULL;
parsedMessage->team = NULL;
parsedMessage->hint = NULL;
parsedMessage->clue = NULL;
parsedMessage->secret = NULL;
parsedMessage->text = NULL;

// arbitrary value assignments to check for duplicate fields later
//parsedMessage->lastContact = NULL;
parsedMessage->latitude = NULL;
parsedMessage->longitude = NULL;
parsedMessage->num_players = NULL;
parsedMessage->num_claimed = NULL;
parsedMessage->num_krags = NULL;
parsedMessage->status_req = NULL;

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
static int parse_helper(char *message, p_message_t *parsed_message)
{
	
	char *field;
	char *delim = "|";

	field = my_strtok(message, delim);

	// assign instances of the struct to the different types
	while (field != NULL) {
		APP_LOG(APP_LOG_LEVEL_INFO, "%s", field);
		if (strncmp(field, "opCode=", 7) == 0) {
			if (parsed_message->op_code != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate opCode field given, message ignored\n");
				return 1;
			}

			parsed_message->op_code = malloc(strlen(field) - 6);

			strcpy(parsed_message->op_code, field + 7);
			strcat(parsed_message->op_code, "\0");
			APP_LOG(APP_LOG_LEVEL_ERROR, "***opcode: %s", parsed_message->op_code);
		}

		else if (strncmp(field, "respCode=", 9) == 0) {
			if (parsed_message->resp_code != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate respCode field given, message ignored\n");
				return 1;
			}

			parsed_message->resp_code = malloc(strlen(field) - 8);
			strcpy(parsed_message->resp_code, field + 9);
			strcat(parsed_message->resp_code, "\0");
		}

		else if (strncmp(field, "kragId=", 7) == 0) {
			if (parsed_message->krag_id != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate kragId field given, message ignored\n");
				return 1;
			}

			parsed_message->krag_id = malloc(strlen(field) - 6);
			strcpy(parsed_message->krag_id, field + 7);
			strcat(parsed_message->krag_id, "\0");
		}

		else if (strncmp(field, "gameId=", 7) == 0) {
			if (parsed_message->game_id != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate gameId field given, message ignored\n");
				return 1;
			}

			parsed_message->game_id = malloc(strlen(field) - 6);
			strcpy(parsed_message->game_id, field + 7);
			strcat(parsed_message->game_id, "\0");
		}

		else if (strncmp(field, "guideId=", 8) == 0) {
			if (parsed_message->guide_id != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate guideId field given, message ignored\n");
				return 1;
			}

			parsed_message->guide_id = malloc(strlen(field) - 7);
			strcpy(parsed_message->guide_id, field + 8);
			strcat(parsed_message->guide_id, "\0");
		}

		else if (strncmp(field, "pebbleId=", 9) == 0) {
			if (parsed_message->pebble_id != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate pebbleId field given, message ignored\n");
				return 1;
			}

			parsed_message->pebble_id = malloc(strlen(field) - 8);
			strcpy(parsed_message->pebble_id, field + 9);
			strcat(parsed_message->pebble_id, "\0");
		}

		else if (strncmp(field, "latitude=", 9) == 0) {
			if (parsed_message->latitude != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate latitude field given, message ignored\n");
				return 1;
			}

			parsed_message->latitude = malloc(strlen(field) - 8);
			strcpy(parsed_message->latitude, field + 9);
			strcat(parsed_message->latitude, "\0");
		}

		else if (strncmp(field, "longitude=", 10) == 0) {
			if (parsed_message->longitude != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate longitude field given, message ignored\n");
				return 1;
			}

			parsed_message->longitude = malloc(strlen(field) - 9);
			strcpy(parsed_message->longitude, field + 10);
			strcat(parsed_message->longitude, "\0");
		}

		else if (strncmp(field, "numPlayers=", 11) == 0) {
			if (parsed_message->num_players != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate numPlayers field given, message ignored\n");
				return 1;
			}

			parsed_message->num_players = malloc(strlen(field) - 10);
			strcpy(parsed_message->num_players, field + 11);
			strcat(parsed_message->num_players, "\0");
		}

		else if (strncmp(field, "numClaimed=", 11) == 0) {
			if (parsed_message->num_claimed != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate numClaimed field given, message ignored\n");
				return 1;
			}

			parsed_message->num_claimed = malloc(strlen(field) - 10);
			strcpy(parsed_message->num_claimed, field + 11);
			strcat(parsed_message->num_claimed, "\0");
		}

		else if (strncmp(field, "numKrags=", 9) == 0) {
			if (parsed_message->num_krags != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate numKrags field given, message ignored\n");
				return 1;
			}

			parsed_message->num_krags = malloc(strlen(field) - 8);
			strcpy(parsed_message->num_krags, field + 9);
			strcat(parsed_message->num_krags, "\0");
		}

		else if (strncmp(field, "statusReq=", 10) == 0) {
			if (parsed_message->status_req != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate statusReq field given, message ignored\n");
				return 1;
			}

			parsed_message->status_req = malloc(strlen(field) - 9);
			strcpy(parsed_message->status_req, field + 10);
			strcat(parsed_message->status_req, "\0");
		}

		else if (strncmp(field, "player=", 7) == 0) {
			if (parsed_message->player != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate player field given, message ignored\n");
				return 1;
			}

			parsed_message->player = malloc(strlen(field) - 6);
			strcpy(parsed_message->player, field + 7);
			strcat(parsed_message->player, "\0");
		}

		else if (strncmp(field, "team=", 5) == 0) {
			if (parsed_message->team != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate team field given, message ignored\n");
				return 1;
			}

			parsed_message->team = malloc(strlen(field) - 4);
			strcpy(parsed_message->team, field + 5);
			strcat(parsed_message->team, "\0");
		}

		else if (strncmp(field, "hint=", 5) == 0) {
			if (parsed_message->hint != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate hint field given, message ignored\n");
				return 1;
			}

			parsed_message->hint = malloc(strlen(field) - 4);
			strcpy(parsed_message->hint, field + 5);
			strcat(parsed_message->hint, "\0");
		}

		else if (strncmp(field, "clue=", 5) == 0) {
			if (parsed_message->clue != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate clue field given, message ignored\n");
				return 1;
			}

			parsed_message->clue = malloc(strlen(field) - 4);
			strcpy(parsed_message->clue, field + 5);
			strcat(parsed_message->clue, "\0");
		}

		else if (strncmp(field, "secret=", 7) == 0) {
			if (parsed_message->secret != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate secret field given, message ignored\n");
				return 1;
			}

			parsed_message->secret = malloc(strlen(field) - 7);
			strcpy(parsed_message->secret, field + 7);
		}

		else if (strncmp(field, "text=", 5) == 0) {
			if (parsed_message->text != NULL) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "duplicate team field given, message ignored\n");
				return 1;
			}

			parsed_message->text = malloc(strlen(field) - 4);
			strcpy(parsed_message->text, field + 5);
			strcat(parsed_message->text, "\0");
		}

		else {
			APP_LOG(APP_LOG_LEVEL_ERROR, "invalid fieldName\n");
			return 2;
		}

		field = my_strtok(NULL, delim);
	}

	return 0;
}

void delete_message(p_message_t *message) {
	if (message->op_code != NULL){
		free(message->op_code);
	}

	if (message->resp_code != NULL) {
		free(message->resp_code);
	}

	if (message->krag_id != NULL) {
		free(message->krag_id);
	}

	if (message->game_id != NULL) {
		free(message->game_id);
	}

	if (message->guide_id != NULL) { 
		free(message->guide_id);
	}

	if (message->pebble_id != NULL) {
		free(message->pebble_id);
	}

	//if (message->last_contact != NULL) {
	//	free(message->last_contact);
	//}

	if (message->latitude != NULL) {
		free(message->latitude);
	}

	if (message->longitude != NULL) {
		free(message->longitude);
	}

	if (message->num_players != NULL) {
		free(message->num_players);
	}

	if (message->num_claimed != NULL) {
		free(message->num_claimed);
	}

	if (message->num_krags != NULL) {
		free(message->num_krags);
	}

	if (message->status_req != NULL) {
		free(message->status_req);
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
my_strtok(char *s, char *delim)
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
