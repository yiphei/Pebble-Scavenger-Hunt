/*
 * display.c - implements the graphical interface for the Guide Agent
 * using ncurses. Used to abstract the interface functionality from 
 * the Guide Agent so that it only need worry about game logic
 *
 *
 * GREP, CS50, May 2017
 */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include "display.h"

#define CTRL(c)  ((c)-0100)  // control character
static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

int map_uy = 30;
int map_ux = 60;
int map_ly = 0;
int map_lx = 0;
int stats_uy = 10;
int stats_ux = 20;

WINDOW * map;
WINDOW * stringWin;
WINDOW * statsWin;
WINDOW * cluesWin;
WINDOW * inputWin;


//source  http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html
WINDOW * createWin_I(int height, int width, int starty, int startx)
{	
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/

	wrefresh(local_win);		/* Show that box 		*/
	return local_win;
}



void initializeWindows_I(void){
	int x,y;
	map =createWin_I(map_uy, map_ux, 0, 0);  //for map

	statsWin =  createWin_I(stats_uy, stats_ux, 0, map_ux);   //for game stats
	getbegyx(statsWin, y,x);
	wmove(statsWin, y, x);
	wprintw(statsWin, "GAME STATS");
	wrefresh(statsWin);

	stringWin = createWin_I(stats_uy, stats_ux, stats_uy, map_ux);  //for curr stirng
	getbegyx(stringWin, y,x);
	wmove(stringWin, y, x);
	wprintw(stringWin, "CURRENT STRING");
	wrefresh(stringWin);

	cluesWin = createWin_I(stats_uy, stats_ux, stats_uy + stats_uy, map_ux); //list of all clues
	getbegyx(cluesWin, y,x);
	wmove(cluesWin, y, x);
	wprintw(cluesWin, "CLUES");
	wrefresh(cluesWin);

	inputWin = createWin_I(stats_uy, stats_ux + map_ux, map_uy, 0); //list of all clues
	getbegyx(inputWin, y,x);
	wmove(inputWin, y, x);
	wprintw(inputWin, "INPUT");
	wrefresh(inputWin);
}


void updateString_I(char * revealedString){

	mvwprintw(stringWin, 2, 1,  "%s", revealedString);
	wrefresh(stringWin);
}


void updateClue_I(char * clue){

	mvwprintw(stringWin, 2, 1,  "%s", revealedString);
	wrefresh(stringWin);
}


// caller must free pointer
char * input_I(void){

	int c; //the character

	char * string = malloc(50);

	int x;
  	wmove(inputWin,1,1);

  	int pos = 0;

  	while ((c = wgetch(inputWin)) != CTRL('D')) {
		if (isprint(c)) { 
			waddch(inputWin, c); ++x; 
  			 string = (char *) realloc(string, strlen(string) + 1 );
  			 string[pos++] = (char) c;
		}
    wrefresh(inputWin);
  }

  wrefresh(inputWin);
  string[pos] = '\0';
  refresh();
  return string;
}













