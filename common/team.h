/* 
 * team.h - header file for 'team' module
 * 
 * The team module contains all the logic for creating teams, adding guide agents and field
 * agents, and related functions. The module build a hashtable of teams where the key
 * is the teamname and the item is a team struct. In the team struct itself, guide agent,
 * and a set of field agents among other things are stored. To initialize the hashtable,
 * call initHash().
 *
 *
 * GREP
 */

#ifndef __TEAM_H
#define __TEAM_H
#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "krag.h"
#include "network.h"

// #ifndef LOG_H
// #define LOG_H
// #endif

/**************** global types ****************/
typedef struct team {
  struct guideAgent * guideAgent;  //guide agent of the team
  set_t * FAset;      //set of field agents
  set_t * FAPebbleIds; // set of pebble Ids with names as items
  char * revealedString;  //the current releaved string of the team
  set_t * krags;   //set of all krags the team has found
  char * recentClues[2];    //array of the two most recent clues
  set_t * clues;   //set of all the clues a team has
  int claimed;  //number of claimed krags of the team
  int numPlayers; // number of players on the team
} team_t;


typedef struct guideAgent { 
	char * guideID;  
 	char * name;    //name of the guide agent
 	char * gameID; 
 	connection_t * conn; //connection struct from network module   
} guideAgent_t;


typedef struct fieldAgent {
	double latitude;  //latitude position of the field agent
	double longitude;  //longitude position of the field agent
	char * gameID;
	char pebbleID[9]; 
	connection_t * conn;  //conection struct form network module
	int lastContact;  //number of seconds since guide agent last heard from field agent
} fieldAgent_t;
/**************** functions ****************/

/*
* This fucntion returns the revealed string of a team. 
*/
char * getRevealedString(char * teamname, hashtable_t * teamhash);

/*
* This function initializes a hashtable of teams and returns an empty hashtable.
* THis function should be called at the beginning of the game to initialize the hastable. 
* Caller is rensposible for freeing this pointer.
*/
hashtable_t * initHash(void);

/*
* This function adds a field agent to a team. If the field agent is being added to a non-existing team,
* than a team will be created first, and then the field agent will be added to the team. In a normal 
* situation, the function will return 0. If the user tries to add a field agent to a team that already
* has a field agent with the same name, then nothing happens and it returns 1.
*/
int addFieldAgent(char * name, char * pebbleID, char * teamname, char * gameID, connection_t * conn, hashtable_t * teamhash);

/*
* This function adds a guide agent to a team. If the guide agent is being added to a non-existing team,
* than a team will be created first, and then the guide agent will be added to the team. In a normal 
* situation, the function will return 0. If the user tries to add a guide agent to a team that already
* has a guide agent, then nothing happens and it returns 1.
*/
int addGuideAgent(char * guideID, char * teamname, char * name, char * gameID, connection_t * conn, hashtable_t * teamhash);

/*
* This function adds a krag to the set of krags found by a team. This function
* should be called when a team find a krag. If the krag added is a new krag, then
* the krag will be added to the sts of krags found by a team and return 0.
* If the krags added has already been found by the team, then
* nothing is added and func returns 1.
*/
int addKrag(char * teamname, char * kragID, hashtable_t * kraghash, hashtable_t * teamhash );

/*
* This function returns the guide agent of the team
*/
guideAgent_t * getGuideAgent(char * teamname, hashtable_t * teamhash);

/*
* This function returns the field agent of the team
*/
fieldAgent_t * getFieldAgent(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all field agents in a team
*/
set_t * getAllFieldAgents(char * teamname, hashtable_t * teamhash);

/*
* This function returns the gameID of a guide agent of a team
*/
char * getGameIDGuidedA( char * teamname, hashtable_t * teamhash);

/*
* This function returns the gameID of a field agent of a team
*/
char * getGameIDFieldA(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function returns the number of krags claimed by the team.
*/
int getKragsClaimed ( char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all the krags found by a team.
*/
set_t * getKrags(char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all clues that a team has
*/
set_t * getClues(char * teamname, hashtable_t * teamhash);

/*
* This function returns the most recent clue that a team has
*/
char * getClueOne(char * teamname, hashtable_t * teamhash);

/*
* This function returns the second-most recent clue that a team has
*/
char * getClueTwo(char * teamname, hashtable_t * teamhash);

/*
* This function updates the location of a field agent. If the team does not exists or the field agent
* does not exists, then nothing is done.
*/
void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash);

/*
* This function increments time since guide agent last heard from a field agent
*/
void incrementTime(char * name, char * teamname, hashtable_t * teamhash);

/*
* This fucntions returns the time since guide agent last heard from a field agent
*/
int getTime(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function frees memory of the hashtable and everything in it.
*/
void deleteTeamHash(hashtable_t * teamhash);


void deleteTeamHashGA(hashtable_t * teamhash);

/*
* This function creates a new field agent.
*/
fieldAgent_t * newFieldAgent(char * gameID, char * pebbleID, connection_t * conn);

/*
* This function creates a new guide agent
*/
guideAgent_t * newGuideAgent(char * guideID, char * name, char * gameID, connection_t * conn);


/*
* This function creates a new team
*/
team_t * newTeam(void);

/*
* This function prints all the teams and all of its members
*/
void printTeams(hashtable_t * teamhash);


#endif // __TEAM_H
