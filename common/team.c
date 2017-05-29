/* 
 * team.c -  'team' module
 *
 * see team.h for more information.
 *
 * GREP
 */
//#ifdef NOPEBBLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "team.h"

char * getRevealedString(char * teamname, hashtable_t * teamhash){
	team_t * team = hashtable_find(teamhash, teamname); 
	return team->revealedString;
}

hashtable_t * initHash(void){
	hashtable_t * teamhash = hashtable_new(50);  //initialize a new hashtable of teams
	return teamhash;
}

int addFieldAgent(char * name, char * pebbleID, char * teamname, char * gameID, connection_t * conn, hashtable_t * teamhash){

	//create the team if it does not exist
	if (hashtable_find(teamhash, teamname) == NULL){
		team_t * newteam = newTeam();
		hashtable_insert(teamhash, teamname, newteam);
	}

	team_t * newteam = hashtable_find(teamhash, teamname); 

	//if the function already has a field agent with the same name
	if (set_find(newteam->FAset, name) != NULL){
		return 1;
	}
	fieldAgent_t * newFA = newFieldAgent(gameID, pebbleID, conn);  //initialize a new field agent
	set_insert(newteam->FAset, name, newFA);

	// insert into pebble ID set
	set_insert(newteam->FAPebbleIds, pebbleID, name);

	// increment the numPlayers
	newteam->numPlayers = newteam->numPlayers + 1;
	return 0;
}

int addGuideAgent(char * guideID, char * teamname, char * name, char * gameID, connection_t * conn, hashtable_t * teamhash){

	//create the team if it does not exists
	if (hashtable_find(teamhash, teamname) == NULL){
		team_t * newteam = newTeam();
		hashtable_insert(teamhash, teamname, newteam);
	}

	//if the funciton already has a guide agent
	if ( ((team_t *)(hashtable_find(teamhash, teamname)))->guideAgent != NULL){
		return 1;
	}

	//create a new guide agent and assign it to the team
	team_t * newteam = hashtable_find(teamhash, teamname);
	guideAgent_t * newGA = newGuideAgent(guideID, name , gameID, conn);
	newteam->guideAgent = newGA;

	newteam->numPlayers = newteam->numPlayers + 1;

	return 0;
}

int addKrag(char * teamname, char * kragID, hashtable_t * kraghash, hashtable_t * teamhash ){

	team_t * team = hashtable_find(teamhash, teamname);
	krag_t * krag = hashtable_find(kraghash, kragID);

	//check if krag has already been found
	if (set_find(team->krags, kragID) != NULL){
		return 1;
	}
	set_insert(team->krags, kragID, krag);
	return 0;
}

guideAgent_t * getGuideAgent(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->guideAgent;
}

fieldAgent_t * getFieldAgent(char * name, char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return set_find(team->FAset, name);
}

set_t * getAllFieldAgents(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->FAset;
}

char * getGameIDFieldA(char * name, char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	fieldAgent_t * fa =  set_find(team->FAset, name);
	return fa->gameID;
}

char * getGameIDGuidedA( char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return (team->guideAgent)->gameID;
}

int getKragsClaimed ( char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->claimed;
}

set_t * getKrags(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->krags;
}

set_t * getClues(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->clues;
}

char * getClueOne(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->recentClues[0];
}

char * getClueTwo(char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	return team->recentClues[1];
}


void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	//process only if team and field agent exist
	if (team != NULL && set_find(team->FAset, name) != NULL){
		fieldAgent_t * FA = set_find(team->FAset, name);
		FA->latitude = latitude;
		FA->longitude = longitude;
	}
}


void incrementTime(char * name, char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	fieldAgent_t * fa = set_find(team->FAset, name);
	fa->lastContact = fa->lastContact + 1;
}


int getTime(char * name, char * teamname, hashtable_t * teamhash){

	team_t * team = hashtable_find(teamhash, teamname);
	fieldAgent_t * fa = set_find(team->FAset, name);
	return fa->lastContact;
}

//Helper function to free field agents
static void deleteFA(void * item){
	if (item){
		free(((fieldAgent_t *) item)->gameID);
		deleteConnection(((fieldAgent_t *) item)->conn);
		free((fieldAgent_t *) item);
	}
}

//helper functino to free krags
static void deleteKrags(void * item){

	if (item){
	}
}

//helper functino to free clues
static void deleteClues(void * item){

	if (item){
	}
}

//helper functino to free pebble ids
static void deletePebble(void * item){

	if (item){
	}
}

//helper function to free team struct
static void deleteTeam(void *item)
{
  if ((team_t *)item != NULL) {

  	if (((team_t *)item)->revealedString != NULL){
  		free(((team_t *)item)->revealedString);
  	}

  	if ((((team_t *)item)->guideAgent)->gameID != NULL){

	  	free((((team_t *)item)->guideAgent)->guideID);
	  	free((((team_t *)item)->guideAgent)->name);
	  	free((((team_t *)item)->guideAgent)->gameID);
  	}
  	deleteConnection((((team_t *)item)->guideAgent)->conn);
  	free(((team_t *)item)->guideAgent);
  	set_delete(((team_t *)item)->FAset, deleteFA);
  	set_delete(((team_t *)item)->krags, deleteKrags);
  	set_delete(((team_t *)item)->clues, deleteClues);
  	set_delete(((team_t *)item)->FAPebbleIds, deletePebble);

  	//free the array
	for (int i=0; i<2; i++) {
		if (((team_t *)item)->recentClues[i] != NULL) {
			free(((team_t *)item)->recentClues[i]);
		}
	}
  	free((team_t *)item);
  }
}

void deleteTeamHash(hashtable_t * teamhash){
	if (teamhash != NULL){
		hashtable_delete(teamhash, deleteTeam);
	}
}

fieldAgent_t * newFieldAgent(char * gameID, char * pebbleID, connection_t * conn){
	fieldAgent_t * fa = malloc(sizeof(fieldAgent_t));
  	if (fa == NULL) {
    	return NULL; // error allocating field agent
  	} 
  	else {
  	fa->gameID = malloc(strlen(gameID)+1);
  	strcpy(fa->gameID,gameID);
  	strcpy(fa->pebbleID,pebbleID);
  	fa->conn = conn;
  	fa->lastContact = 0;
    return fa;
 }
}

guideAgent_t * newGuideAgent(char * guideID, char * name, char * gameID, connection_t * conn){

	guideAgent_t * ga = malloc(sizeof(guideAgent_t));
  	if (ga == NULL) {
    	return NULL; // error allocating guide agent
  	} else {

    // initialize contents of guide agent structure
  	ga->name = malloc(strlen(name)+1);
  	ga->guideID = malloc(strlen(guideID)+1);
  	ga->gameID = malloc(strlen(gameID)+1);  
  	strcpy(ga->name,name);
  	strcpy(ga->guideID,guideID);
  	strcpy(ga->gameID,gameID);
  	ga->conn = conn;

    return ga;
 }
}


team_t * newTeam(void){

  team_t * team = malloc(sizeof(team_t));
  if (team == NULL) {
    return NULL; // error allocating team
  } else {
    // initialize contents of team structure
    team->revealedString = NULL;
  	team->FAset = set_new();
  	team->guideAgent = NULL;
  	team->krags = set_new();
  	team->clues = set_new();
  	team->numPlayers = 0;
  	team->FAPebbleIds = set_new();
  	team->guideAgent = NULL;

  	for (int i =0; i<2; i++) {
		team->recentClues[i] = NULL;
	}
    return team;
 }
}

//helper function to print field agents
static void FAPrint(FILE *fp, const char *key, void *item)
{
  fieldAgent_t * fa = item;
  if (fa != NULL){
    fprintf(fp, "Name: %s\n", key);
   	}
}

//helper funciton to print teams
static void teamPrint(FILE *fp, const char *key, void *item)
{
  team_t * team = item;
  if (team != NULL){
    fprintf(fp, "Team name: %s\n", key);
    fprintf(fp, "Guide agent name: %s    guideID: %s\n", (team->guideAgent)->name, (team->guideAgent)->guideID);
    fprintf(fp, "Field agents:");
    set_print(team->FAset, stdout, FAPrint);
   	}
}


void printTeams(hashtable_t * teamhash){

	hashtable_print(teamhash, stdout, teamPrint);
}

//#endif

