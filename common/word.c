/*
 * word.c - common functions to modify input strings
 * 
 * to be used in indexer and querier
 *
 * Michael Perezous, CS50, May 2017
 */

//#ifdef NOPEBBLE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

void NormalizeWord(char *word) {
	char c;

	for (int i=0; (c = word[i]) != '\0'; i++) {
		word[i] = tolower(c);
	}
	
}
//#endif
