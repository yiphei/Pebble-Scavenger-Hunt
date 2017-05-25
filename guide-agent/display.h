/* 
 * display.h - header file for 'display' module
 * 
 *
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

void initialize_curses();


WINDOW *createWin_I(int height, int width, int starty, int startx);

void initializeWindows_I(void);

void updateMap_I(set_t * fieldagents);

void addPlayers_I(set_t * fieldagents);

void loadMap_I(void);


void updateString_I(char * revealedString);

void updateClues_I(set_t * clues);


void updateTotalKrags_I(int totalKrags);


void updateKragsClaimed_I(int claimed);


char * input_I(void);

void gameOver_I(hashtable_t * teamhash);




#endif // __DISPLAY_H