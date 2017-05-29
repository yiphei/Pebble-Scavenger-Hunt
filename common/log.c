/*
 * log.c - shared function to log activity to a logfile based on given format
 *
 *
 * GREP, CS50, May 2017
 */

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

void logMessage(char *filePath, char *message, char *direction, connection_t *connect)
{
	FILE *log = fopen(filePath, "a");

	// couldn't open log file
	if (log == NULL) {
		return;
	}

	// get timestamp
	char timestamp[27];
	time_t clk = time(NULL);
	sprintf(timestamp, "(%s", ctime(&clk));
	timestamp[25] = ')';

	// get ip address
	struct sockaddr *addrp = (struct sockaddr *) &connect->address;
	struct sockaddr_in *address = (struct sockaddr_in *)addrp;

	char *ip = inet_ntoa(address->sin_addr);

	// get port number
	int port = connect->socket;

	fprintf(log, "%s %s %s@%d: %s \n", timestamp, direction, ip, port, message);

	fclose(log);
}
