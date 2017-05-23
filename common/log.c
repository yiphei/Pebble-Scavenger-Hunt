/*
 * log.c - shared function to log activity to a logfile based on given format
 *
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void logMessage(FILE *file, char *message, struct connection *connect)
{
	// get timestamp
	char timestamp[27];
	time_t clk = time(NULL);
	sprintf(timestamp, "(%s", ctime(&clk));
	timestamp[25] = ')';

}