/*
 * message.c - common module to parse messages into arrays of strings,
 * implementing a message struct
 *
 * GREP, CS50, May 2017
 */

// message struct allows storage of all message field types
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
	int errorCode;
} message_t;

/* parses messages with the help of a helper function, that will check 
each message field for its type and assign the rest of the message field 
to a specific variable inside of the message struct to be returned */
message_t * parseMessage(char *message);

/* goes through every possible allocated space of a specific message,
specifically all message types, and frees those that are not NULL, 
then frees the message struct itself */
void deleteMessage(message_t *message);