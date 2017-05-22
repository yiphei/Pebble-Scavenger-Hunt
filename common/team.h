/* 
 * team.h - header file for 'team' module
 * 
 *
 *
 * GREP
 */

#ifndef __TEAM_H
#define __TEAM_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"

/**************** global types ****************/
typedef struct team {
  struct guideAgent * guideAgent;  //guide agent of the team
  set_t * FAset;      //set of field agents
  char * revealedString;  //the current releaved string of the team
  set_t * clues;
  int claimed;  //number of claimed krags of the team
} team_t;

typedef struct guideAgent guideAgent_t;
typedef struct fieldAgent fieldAgent_t;
/**************** functions ****************/

/*
* This fucntion returns the revealed string of a team. 
*/
char * getRevealedString(char * teamname, hashtable_t * teamhash);


/*
* This function initializes a hashtable of teams and returns an empty hashtable. 
* Caller is rensposible for freeing this pointer.
*/
hashtable_t * init(void);

/*
* This function adds a field agent to a team. If the field agent is being added to a non-existing team,
* than a team will be created first, and then the field agent will be added to the team.
*/
void addFieldAgent(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function adds a guide agent to a team. If the guide agent is being added to a non-existing team,
* than a team will be created first, and then the guide agent will be added to the team. In a normal 
* situation, the function will return 0. If the user tries to add a guide agent to a team that already
* has a guide agent, then nothing happens and it returns 1.
*/
int addGuideAgent(char * guideID, char * teamname, char * name, hashtable_t * teamhash);

/*
* This function updates the location of a field agent. If the team does not exists or the field agent
* does not exists, then nothing is done.
*/
void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash);

/*
* This function frees memory of the hashtable and everything in it.
*/
void deleteTeamHash(hashtable_t * teamhash);

/*
* This function creates a new field agent.
*/
fieldAgent_t * newFieldAgent(void);

/*
* This function creates a new guide agent
*/
guideAgent_t * newGuideAgent(char * guideID, char * name);

/*
* This function creates a new team
*/
team_t * newTeam(void);

/*
* This function prints all the teams and all of its members
*/
void printTeams(hashtable_t * teamhash);


#endif // __TEAM_H