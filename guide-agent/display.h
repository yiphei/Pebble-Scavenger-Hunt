/* 
 * display.h - header file for 'display' module
 * 
 * The display module handles the GUI part of the guide agent. It displays five windows in total. 
 * The map window will have field agents name on the top left, their current locations in the map, 
 * and a ASCII based map of the campus. The stats window will display the total number of krags in 
 * the game and the total nuber of krags claimed. The current string windown displays the current 
 * revealed string of the team. The clues window displays all the known clues to the team. The input
 * window reads from stdin. When game ends, then it will display a game over
 *
 * GREP
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include "../common/team.h"
#include "../libcs50/set.h"

/*
* initialize curses like screen and color. This function should be called first in order 
* to call all other functions in the module
* From life.c program of David Kotz.
*/
void initialize_curses();

/*
* This function creates and returns a window given a height, width, and start x and y coordinates
* From NCURSES Programming HOWTO http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html
*/
WINDOW *createWin_I(int height, int width, int starty, int startx);

/*
*This function initialites the five windows of the game and print their window names on the top 
*left of each window. These windows are: game, game status, string, clues, and input. This function
* should be called after initialize_curses()
*/
void initializeWindows_I(void);

/*
* This function displays the campus map with the name of the players and their locatins on the map
* with different colors.  
*/
void updateMap_I(set_t * fieldagents, set_t * krags);

/*
* This function adds the field agents name on the top left corner of the mpa window, and their 
* renspective location (marked with *) on the map. Each player and their location mark have an individual color.
* If there are more than seven field agents, then the colors will repeated since there are only 7 different
* colors available
*/
void addPlayers_I(set_t * fieldagents);

/*
*
*/
void addKrags_I(set_t * krags);

/*
* This function displays the current revealed string of the team to the string window
*/
void updateString_I(char * revealedString);

/*
* This function outputs all the clues to the the clue window. If clues exceed the borders of the clue window,
* then those clues wont be displayed
*/
void updateClues_I(set_t * clues);

/*
* This function displays the total krag number in the game. It is displayed in the stats window
*/
void updateTotalKrags_I(int totalKrags);

/*
* This function displays the number of krags claimed by the team. It is displayed in the stats window
*/
void updateKragsClaimed_I(int claimed);

/*
* This function reads from stdin until user inputs a carriage return. When carriage return is inputted, 
* then the function stops reading frmo stdin and returns the string to the caller. Caller is rensposible
* to free the pointer returned.
*/
char * input_I(void);

/*
* This function displays the game over stats to the screen. For every team, it will diplay the teamname, 
* number of players, and number of krags claimed. This function should be called at the end of the game.
* Then, to terminate GUI, press any key.
*/
void gameOver_I(hashtable_t * teamhash);

#endif // __DISPLAY_H