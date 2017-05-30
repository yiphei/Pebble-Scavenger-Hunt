/*
 * p_message.h - module that is the same as message.h, but
 * everything is a string.  This makes life easier for pebble
 * 
 * Paolo Takagi-Atilano, May 2017
 */

typedef struct p_message 
{
	char *op_code; 
	char *resp_code; 
	char *krag_id; 
	char *game_id; 
	char *guide_id; 
	char *pebble_id; 
	char *latitude; 
	char *longitude; 
	char *num_players;  
	char *num_claimed;  
	char *num_krags;   
	char *status_req;  
	char *player; 
	char *team; 
	char *hint; 
	char *clue; 
	char *secret; 
	char *text; 
	int error_code;
	
} p_message_t;

// parses the mesage
p_message_t *parse_message(char *message);

// deletes the message
void delete_message(p_message_t *message);

// used by main module of field agent, so it is not static for convience
char *my_strtok(char *s, char *delim);