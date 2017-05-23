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


WINDOW *createWin_I(int height, int width, int starty, int startx);

void initializeWindows_I(void);


void updateString_I(char * revealedString);

char * input_I(void);



#endif // __DISPLAY_H