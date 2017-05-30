/*
 * p_message.h - module that is the same as message.h, but
 * everything is a string.  This makes live easier for pebble
 * 
 * Paolo Takagi-Atilano, May 2017
 */

typedef struct p_message 
{
	char *op_code; //**
	char *resp_code; //**
	char *krag_id; //**
	char *game_id; //**
	char *guide_id; //**
	char *pebble_id; //**
	//char *last_contact; // was an int
	char *latitude; //**
	char *longitude; //**
	char *num_players;  // was an int //**
	char *num_claimed;  // was an int //**
	char *num_krags;    // was an int //**
	char *status_req;   // was an int //**
	char *player; //**
	char *team; //**
	char *hint; //**
	char *clue; //**
	char *secret; //**
	char *text; //**
	int error_code;
	
} p_message_t;

p_message_t *parse_message(char *message);

void delete_message(p_message_t *message);

char *my_strtok(char *s, char *delim);