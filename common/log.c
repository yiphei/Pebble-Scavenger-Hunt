/*
 * log.c - shared function to log activity to a logfile based on given format
 *
 *
 * GREP, CS50, May 2017
 */

//#ifdef NOPEBBLE
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>  // socket-related structures                                                                    
#include <time.h>
#ifndef LOG_H
#define LOG_H
#include "network.h"
#endif

void logMessage(FILE *file, char *message, char *direction, connection_t *connect)
{
	// get timestamp
	char timestamp[27];
	time_t clk = time(NULL);
	sprintf(timestamp, "(%s", ctime(&clk));
	timestamp[25] = ')';

	// get ip address
	struct sockaddr_in *address = (struct sockaddr_in *)connect->address;

	char *ip = inet_ntoa(address->sin_addr);

	// get port number
	int port = connect->socket;

	fprintf(file, "%s %s %s@%d: %s \n", timestamp, direction, ip, port, message);
	printf("logged message\n");
}

//#endif