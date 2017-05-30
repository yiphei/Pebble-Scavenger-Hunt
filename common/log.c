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

	char* messageNew = calloc(strlen(message) + 1, 1);
	strcpy(messageNew, message);

	// couldn't open log file
	if (log == NULL) {
		free(messageNew);
		return;
	}

	// get timestamp
	char timestamp[27];
	time_t clk = time(NULL);
	sprintf(timestamp, "(%s", ctime(&clk));
	timestamp[25] = ')';

	if(connect != NULL){
		// get ip address
		struct sockaddr *addrp = (struct sockaddr *) &connect->address;
		struct sockaddr_in *address = (struct sockaddr_in *)addrp;

		char *ip = inet_ntoa(address->sin_addr);

		// get port number
		int port = connect->socket;

		if (ip != NULL && port != 0) {
			fprintf(log, "%s %s %s@%d: %s \n", timestamp, direction, ip, port, messageNew);
		}

		else {
			fprintf(log, "%s (error retrieving connection data) %s\n", timestamp, messageNew);
		}
	}
	else {
		fprintf(log, "%s %s Multiple Addresses %s\n", timestamp, direction, messageNew);
	}

	free(messageNew);
	fclose(log);
}
