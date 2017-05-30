/*
 * guide-agent.c - implements the KRAG game logic for the Guide Agent module
 * to handle input messages and update the interface based on input, and
 * send messages to server based on game status
 *
 *
 * GREP, CS50, May 2017
 */

#ifndef __GUIDEAGENT_H
#define __GUIDEAGENT_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>             // isdigit
#include <netdb.h>             // socket-related structures
#include <time.h>
#include <ncurses.h>
#include "../libcs50/memory.h"
#include "display.h"
#include "../libcs50/set.h"
#include "../common/message.h"
#include "../common/log.h"
#include "../common/word.h"
#include "../common/file.h"

/* 
game uses the network.h common module to create a connection to the Game Server,
opens a log file to be written to over the course of the game, then starts the
game loop. In the game loop, it uses the function dispatch/opCode table to 
handle messages based on the parsed opCode. The message is parsed into a
message_t struct that holds all components of a given message type. This 
further abstracts the message handling away from the game function so game
handles only receiving messages, sending GA_STATUS messages to the Game 
Server every 30 seconds, and requesting the game status from the Game Server
(via GA_STATUS messages) every minute.
 */
int game(char *guideId, char *team, char *player, char *host, int port);

/*
Takes a hint as input and handles it as needed, calling the correct handling
function from the function dispatch table.
*/
int handleMessage(char *messagep, team_t *teamp, connection_t *connection, char *filePath, hashtable_t *teams);

/*
Takes a hint from the user input and handles it as needed, sending it to the
Game Server with sendGA_HINT and the appropriate Field Agent recipient(s).
*/
void handleHint(char *gameId, char *guideId, char *team, char *player, char *hint, connection_t *connection, char *filePath, team_t *teamp);

/* 
sendGA_STATUS inductively creates the message, given the correct
components of the message, and sends the message via network.h's 
sendMessage function and the connection_t pointer passed by game. 
*/
bool sendGA_STATUS(char *gameId, char *guideId, char *team, char *player, char *statusReq, connection_t *connection, char *filePath);

/* 
sendGA_HINT inductively creates the message, given the correct components
of the message, and sends the message via network.h's sendMessage
function and tghe connection_t pointer passed by the game 
*/
bool sendGA_HINT(char *gameId, char *guideId, char *team, char *player, char *pebbleId, char *hint, connection_t *connection, char *filePath);

#endif // __GUIDEAGENT_H
