/*
 * my_dialog_window.h - creates a dialog window for multiple purposes (hints, status, game_over)
 * 
 * much of this code was inpired from: (I changed parts to suit my needs)
 * https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/dialog_message_window.h
 * 
 * Paolo Takagi-Atilano, May 2017
 */


#include <pebble.h>

// See link
//#define TYPE_MESSAGE_WINDOW_MARGIN 5
#define DIALOG_MESSAGE_WINDOW_MARGIN 10

// pushes dialog window onto window stack
void my_dialog_window_push(char *text);

//pops dialog window from window stack
void my_dialog_window_pop(); // need this because main won't have access to the pointer