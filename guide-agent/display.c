/*
 * display.c - implements the graphical interface for the Guide Agent
 * using ncurses. Used to abstract the interface functionality from 
 * the Guide Agent so that it only need worry about game logic
 *
 *
 * GREP, CS50, May 2017
 */


//in the specs, i can tell the user to set a specific size and font

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include "display.h"

static int NROWS;
static int NCOLS;

#define CTRL(c)  ((c)-0100)  // control character
static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

int map_uy = 30;
int map_ux = 60;
int map_ly = 0;
int map_lx = 0;
int stats_uy = 10;
int stats_ux = 20;

WINDOW * mapWin;
WINDOW * stringWin;
WINDOW * statsWin;
WINDOW * cluesWin;
WINDOW * inputWin;


//from life.c David Kotz
void initialize_curses()
{
  // initialize the screen - which defines LINES and COLS
  initscr();

  // cache the size of the window in our global variables
  NROWS = LINES;
  NCOLS = COLS;

  cbreak(); // actually, this is the default
  noecho(); // don't show the characters users type

  // I like yellow on a black background
  start_color();
}


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
	mapWin =createWin_I(map_uy, map_ux, 0, 0);  //for map

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


void updateMap_I(set_t * fieldagents){

	addPlayers_I(fieldagents);
}

static void helper(void *arg, const char *key, void *item)
{

	int *y = (int *)arg;
	(*y)++;
  fieldAgent_t * fa = item;
  double longitude = fa->longitude;
  double latitude = fa->latitude;
  mvwprintw(mapWin, *y, 1,  "%s\n", key);
  mvwprintw(mapWin, latitude, longitude,  "*", key);
	wrefresh(mapWin);

}



void addPlayers_I(set_t * fieldagents){

	int y = 2;
	 init_pair(1,COLOR_RED, COLOR_BLACK); //initializes a pair of color
 	wattron(mapWin, COLOR_PAIR(1));
	set_iterate(fieldagents, &y, helper);
		wattroff(mapWin, COLOR_PAIR(1));
}



void loadMap(char **board, FILE *fp)
{
  const int size = NCOLS+2;  // include room for \n\0
  char line[size];	      // a line of input
  int y = 0;

  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < NROWS) {
    int len = strlen(line);
    if (line[len-1] == '\n') {
      // normal line
      len--; // don't copy the newline
    } else {
      // overly wide line
      len = NCOLS;
      fprintf(stderr, "board line %d too wide for screen; truncated.\r\n", y);
      for (char c = 0; c != '\n' && c != EOF; c = getc(fp))
	; // scan off the excess part of the line
    }
    strncpy(board[y++], line, len);
  }

  if (!feof(fp)) {
    fprintf(stderr, "board too big for screen; truncated to %d lines\r\n", y);
  }
}


void updateString_I(char * revealedString){

	mvwprintw(stringWin, 2, 1,  "%s", revealedString);
	wrefresh(stringWin);
}

//change clues into a set
void updateClue_I(char * clues[]){

	int x = 0;

	int lx, ly, ux, uy;

	getbegyx(cluesWin, ly, lx);
	getmaxyx(cluesWin, uy, ux);
	printf("ly is %d  lx is %d\n", ly, lx);
		printf("uy is %d  ux is %d\n", uy, ux);


	printf("uy is %d  ly is %d\n", uy, ly);

	while (clues[x] != NULL){   //instead of doing null, can set this to be a max size like 6 clues

		printf("in while loop\n");

		mvprintw( ly + 2, lx + 1,  "%s\n", clues[x]);
		refresh();
		x++;
		ly++;
		ux++; //not use at all, but I have it so I dont have compiler warning
	}
}


void updateStats_I(char * gamestats[]){

	int x = 0;
	int y = 2;

	while (gamestats[x] != NULL){

		mvwprintw(statsWin, y, 1,  "%s\n", gamestats[x]);
		wrefresh(statsWin);
		x++;
		y++;
	}
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













