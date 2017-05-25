/*
 * log.c - shared function to log activity to a logfile based on given format
 * given a logfile pointer, message, "TO" or "FROM" and a connection
 *
 * GREP, CS50, May 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>  // socket-related structures
#include <time.h>
#include "network.h"

/* this function logs a given message to a given file in the logfile format in
the requirements spec. It uses the direction parameter to print "TO" or "FROM"
in the log message, so that input should be just "TO" or "FROM"*/
void logMessage(FILE *file, char *message, char *direction, connection_t *connect);
